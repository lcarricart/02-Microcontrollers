/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(net_mqtt_publisher_sample, LOG_LEVEL_DBG);

#include <zephyr/posix/poll.h>
#include <zephyr/posix/arpa/inet.h>

#include <zephyr/kernel.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/mqtt.h>
#include <zephyr/random/random.h>
#if defined(CONFIG_LOG_BACKEND_MQTT)
#include <zephyr/logging/log_backend_mqtt.h>
#endif

#include <string.h>
#include <errno.h>

#include "config.h"
#include "net_sample_common.h"

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>

#if defined(CONFIG_USERSPACE)
#include <zephyr/app_memory/app_memdomain.h>
K_APPMEM_PARTITION_DEFINE(app_partition);
struct k_mem_domain app_domain;
#define APP_BMEM K_APP_BMEM(app_partition)
#define APP_DMEM K_APP_DMEM(app_partition)
#else
#define APP_BMEM
#define APP_DMEM
#endif

/* Buffers for MQTT client. */
static APP_BMEM uint8_t rx_buffer[APP_MQTT_BUFFER_SIZE];
static APP_BMEM uint8_t tx_buffer[APP_MQTT_BUFFER_SIZE];

#if defined(CONFIG_MQTT_LIB_WEBSOCKET)
/* Making RX buffer large enough that the full IPv6 packet can fit into it */
#define MQTT_LIB_WEBSOCKET_RECV_BUF_LEN 1280

/* Websocket needs temporary buffer to store partial packets */
static APP_BMEM uint8_t temp_ws_rx_buf[MQTT_LIB_WEBSOCKET_RECV_BUF_LEN];
#endif

/* The mqtt client struct */
static APP_BMEM struct mqtt_client client_ctx;

/* MQTT Broker details. */
static APP_BMEM struct sockaddr_storage broker;

#if defined(CONFIG_SOCKS)
static APP_BMEM struct sockaddr socks5_proxy;
#endif

static APP_BMEM struct pollfd fds[1];
static APP_BMEM int nfds;
static APP_BMEM bool connected;

/* Whether to include full topic in the publish message, or alias only (MQTT 5). */
static APP_BMEM bool include_topic;
static APP_BMEM bool aliases_enabled;

#define APP_TOPIC_ALIAS 1

#define APP_MQTT_TOPIC "python/mqtt"
#define APP_IMU_PAYLOAD_SIZE 256

static const struct device *const mpu6050_dev =
	DEVICE_DT_GET_ONE(invensense_mpu6050);

static APP_BMEM bool imu_is_ready;

static int app_format_sensor_value(char *const buffer,
				   const size_t buffer_size,
				   const struct sensor_value *const value)
{
	bool is_negative;
	int32_t val1_magnitude;
	int32_t val2_magnitude;
	int ret;

	if ((buffer == NULL) || (value == NULL)) {
		ret = -EINVAL;
	} else {
		is_negative = ((value->val1 < 0) || (value->val2 < 0));
		val1_magnitude = value->val1;
		val2_magnitude = value->val2;

		if (val1_magnitude < 0) {
			val1_magnitude = -val1_magnitude;
		}

		if (val2_magnitude < 0) {
			val2_magnitude = -val2_magnitude;
		}

		ret = snprintk(buffer,
			       buffer_size,
			       "%s%d.%06d",
			       is_negative ? "-" : "",
			       val1_magnitude,
			       val2_magnitude);
	}

	return ret;
}

static int app_read_imu_payload(char *const payload, const size_t payload_size)
{
	struct sensor_value accel[3];
	struct sensor_value gyro[3];
	struct sensor_value temperature_value;
	char ax[24];
	char ay[24];
	char az[24];
	char gx[24];
	char gy[24];
	char gz[24];
	char temperature[24];
	int ret;

	if (payload == NULL) {
		return -EINVAL;
	}

	if (!imu_is_ready) {
		ret = snprintk(payload, payload_size, "{\"imu\":\"not_ready\"}");
		return ret;
	}

	ret = sensor_sample_fetch(mpu6050_dev);
	if (ret != 0) {
		(void)snprintk(payload,
			       payload_size,
			       "{\"imu\":\"fetch_error\",\"err\":%d}",
			       ret);
		return ret;
	}

	ret = sensor_channel_get(mpu6050_dev, SENSOR_CHAN_ACCEL_XYZ, accel);
	if (ret != 0) {
		(void)snprintk(payload,
			       payload_size,
			       "{\"imu\":\"accel_error\",\"err\":%d}",
			       ret);
		return ret;
	}

	ret = sensor_channel_get(mpu6050_dev, SENSOR_CHAN_GYRO_XYZ, gyro);
	if (ret != 0) {
		(void)snprintk(payload,
			       payload_size,
			       "{\"imu\":\"gyro_error\",\"err\":%d}",
			       ret);
		return ret;
	}

	ret = sensor_channel_get(mpu6050_dev, SENSOR_CHAN_DIE_TEMP,
				 &temperature_value);
	if (ret != 0) {
		(void)snprintk(payload,
			       payload_size,
			       "{\"imu\":\"temp_error\",\"err\":%d}",
			       ret);
		return ret;
	}

	(void)app_format_sensor_value(ax, sizeof(ax), &accel[0]);
	(void)app_format_sensor_value(ay, sizeof(ay), &accel[1]);
	(void)app_format_sensor_value(az, sizeof(az), &accel[2]);
	(void)app_format_sensor_value(gx, sizeof(gx), &gyro[0]);
	(void)app_format_sensor_value(gy, sizeof(gy), &gyro[1]);
	(void)app_format_sensor_value(gz, sizeof(gz), &gyro[2]);
	(void)app_format_sensor_value(temperature, sizeof(temperature),
				       &temperature_value);

	ret = snprintk(payload,
		       payload_size,
		       "{\"accel\":{\"x\":%s,\"y\":%s,\"z\":%s},"
		       "\"gyro\":{\"x\":%s,\"y\":%s,\"z\":%s},"
		       "\"temp\":%s}",
		       ax, ay, az, gx, gy, gz, temperature);

	return ret;
}

#if defined(CONFIG_MQTT_LIB_TLS)

#include "test_certs.h"

#define TLS_SNI_HOSTNAME "localhost"
#define APP_CA_CERT_TAG 1
#define APP_PSK_TAG 2

static APP_DMEM sec_tag_t m_sec_tags[] = {
#if defined(CONFIG_MBEDTLS_X509_CRT_PARSE_C) || defined(CONFIG_NET_SOCKETS_OFFLOAD)
		APP_CA_CERT_TAG,
#endif
#if defined(MBEDTLS_KEY_EXCHANGE_SOME_PSK_ENABLED)
		APP_PSK_TAG,
#endif
};

static int tls_init(void)
{
	int err = -EINVAL;

#if defined(CONFIG_MBEDTLS_X509_CRT_PARSE_C) || defined(CONFIG_NET_SOCKETS_OFFLOAD)
	err = tls_credential_add(APP_CA_CERT_TAG, TLS_CREDENTIAL_CA_CERTIFICATE,
				 ca_certificate, sizeof(ca_certificate));
	if (err < 0) {
		LOG_ERR("Failed to register public certificate: %d", err);
		return err;
	}
#endif

#if defined(MBEDTLS_KEY_EXCHANGE_SOME_PSK_ENABLED)
	err = tls_credential_add(APP_PSK_TAG, TLS_CREDENTIAL_PSK,
				 client_psk, sizeof(client_psk));
	if (err < 0) {
		LOG_ERR("Failed to register PSK: %d", err);
		return err;
	}

	err = tls_credential_add(APP_PSK_TAG, TLS_CREDENTIAL_PSK_ID,
				 client_psk_id, sizeof(client_psk_id) - 1);
	if (err < 0) {
		LOG_ERR("Failed to register PSK ID: %d", err);
	}
#endif

	return err;
}

#endif /* CONFIG_MQTT_LIB_TLS */

static void prepare_fds(struct mqtt_client *client)
{
	if (client->transport.type == MQTT_TRANSPORT_NON_SECURE) {
		fds[0].fd = client->transport.tcp.sock;
	}
#if defined(CONFIG_MQTT_LIB_TLS)
	else if (client->transport.type == MQTT_TRANSPORT_SECURE) {
		fds[0].fd = client->transport.tls.sock;
	}
#endif

	fds[0].events = POLLIN;
	nfds = 1;
}

static void clear_fds(void)
{
	nfds = 0;
}

static int wait(int timeout)
{
	int ret = 0;

	if (nfds > 0) {
		ret = poll(fds, nfds, timeout);
		if (ret < 0) {
			LOG_ERR("poll error: %d", errno);
		}
	}

	return ret;
}

void mqtt_evt_handler(struct mqtt_client *const client,
		      const struct mqtt_evt *evt)
{
	int err;

	switch (evt->type) {
	case MQTT_EVT_CONNACK:
		if (evt->result != 0) {
			LOG_ERR("MQTT connect failed %d", evt->result);
			break;
		}

		connected = true;
		LOG_INF("MQTT client connected!");

#if defined(CONFIG_MQTT_VERSION_5_0)
		if (evt->param.connack.prop.rx.has_topic_alias_maximum &&
		    evt->param.connack.prop.topic_alias_maximum > 0) {
			LOG_INF("Topic aliases allowed by the broker, max %u.",
				evt->param.connack.prop.topic_alias_maximum);

			aliases_enabled = true;
		} else {
			LOG_INF("Topic aliases disallowed by the broker.");
		}
#endif

#if defined(CONFIG_LOG_BACKEND_MQTT)
		log_backend_mqtt_client_set(client);
#endif

		break;

	case MQTT_EVT_DISCONNECT:
		LOG_INF("MQTT client disconnected %d", evt->result);

		connected = false;
		clear_fds();

#if defined(CONFIG_LOG_BACKEND_MQTT)
		log_backend_mqtt_client_set(NULL);
#endif

		break;

	case MQTT_EVT_PUBACK:
		if (evt->result != 0) {
			LOG_ERR("MQTT PUBACK error %d", evt->result);
			break;
		}

		LOG_INF("PUBACK packet id: %u", evt->param.puback.message_id);

		break;

	case MQTT_EVT_PUBREC:
		if (evt->result != 0) {
			LOG_ERR("MQTT PUBREC error %d", evt->result);
			break;
		}

		LOG_INF("PUBREC packet id: %u", evt->param.pubrec.message_id);

		const struct mqtt_pubrel_param rel_param = {
			.message_id = evt->param.pubrec.message_id
		};

		err = mqtt_publish_qos2_release(client, &rel_param);
		if (err != 0) {
			LOG_ERR("Failed to send MQTT PUBREL: %d", err);
		}

		break;

	case MQTT_EVT_PUBCOMP:
		if (evt->result != 0) {
			LOG_ERR("MQTT PUBCOMP error %d", evt->result);
			break;
		}

		LOG_INF("PUBCOMP packet id: %u",
			evt->param.pubcomp.message_id);

		break;

	case MQTT_EVT_PINGRESP:
		LOG_INF("PINGRESP packet");
		break;

	default:
		break;
	}
}

static char *get_mqtt_payload(enum mqtt_qos qos)
{
	static APP_DMEM char payload[APP_IMU_PAYLOAD_SIZE];
	int ret;

	ARG_UNUSED(qos);

	ret = app_read_imu_payload(payload, sizeof(payload));
	if (ret < 0) {
		LOG_ERR("Failed to read IMU payload: %d", ret);
	}

	return payload;
}

static char *get_mqtt_topic(void)
{
	static APP_DMEM char topic[] = APP_MQTT_TOPIC;

	return topic;
}

static int publish(struct mqtt_client *client, enum mqtt_qos qos)
{
	struct mqtt_publish_param param = { 0 };

	/* Always true for MQTT 3.1.1.
	 * True only on first publish message for MQTT 5.0 if broker allows aliases.
	 */
	if (include_topic) {
		param.message.topic.topic.utf8 = (uint8_t *)get_mqtt_topic();
		param.message.topic.topic.size =
			strlen(param.message.topic.topic.utf8);
	}

	param.message.topic.qos = qos;
	param.message.payload.data = get_mqtt_payload(qos);
	param.message.payload.len =
			strlen(param.message.payload.data);
	param.message_id = sys_rand16_get();
	param.dup_flag = 0U;
	param.retain_flag = 0U;

#if defined(CONFIG_MQTT_VERSION_5_0)
	if (aliases_enabled) {
		param.prop.topic_alias = APP_TOPIC_ALIAS;
		include_topic = false;
	}
#endif

	return mqtt_publish(client, &param);
}

#define RC_STR(rc) ((rc) == 0 ? "OK" : "ERROR")

#define PRINT_RESULT(func, rc) \
	LOG_INF("%s: %d <%s>", (func), rc, RC_STR(rc))

static void broker_init(void)
{
#if defined(CONFIG_NET_IPV6)
	struct sockaddr_in6 *broker6 = (struct sockaddr_in6 *)&broker;

	broker6->sin6_family = AF_INET6;
	broker6->sin6_port = htons(SERVER_PORT);
	inet_pton(AF_INET6, SERVER_ADDR, &broker6->sin6_addr);

#if defined(CONFIG_SOCKS)
	struct sockaddr_in6 *proxy6 = (struct sockaddr_in6 *)&socks5_proxy;

	proxy6->sin6_family = AF_INET6;
	proxy6->sin6_port = htons(SOCKS5_PROXY_PORT);
	inet_pton(AF_INET6, SOCKS5_PROXY_ADDR, &proxy6->sin6_addr);
#endif
#else
	struct sockaddr_in *broker4 = (struct sockaddr_in *)&broker;

	broker4->sin_family = AF_INET;
	broker4->sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_ADDR, &broker4->sin_addr);
#if defined(CONFIG_SOCKS)
	struct sockaddr_in *proxy4 = (struct sockaddr_in *)&socks5_proxy;

	proxy4->sin_family = AF_INET;
	proxy4->sin_port = htons(SOCKS5_PROXY_PORT);
	inet_pton(AF_INET, SOCKS5_PROXY_ADDR, &proxy4->sin_addr);
#endif
#endif
}

static void client_init(struct mqtt_client *client)
{
	mqtt_client_init(client);

	broker_init();

	/* MQTT client configuration */
	client->broker = &broker;
	client->evt_cb = mqtt_evt_handler;
	client->client_id.utf8 = (uint8_t *)MQTT_CLIENTID;
	client->client_id.size = strlen(MQTT_CLIENTID);
	client->password = NULL;
	client->user_name = NULL;
#if defined(CONFIG_MQTT_VERSION_5_0)
	client->protocol_version = MQTT_VERSION_5_0;
#else
	client->protocol_version = MQTT_VERSION_3_1_1;
#endif

	/* MQTT buffers configuration */
	client->rx_buf = rx_buffer;
	client->rx_buf_size = sizeof(rx_buffer);
	client->tx_buf = tx_buffer;
	client->tx_buf_size = sizeof(tx_buffer);

	/* MQTT transport configuration */
#if defined(CONFIG_MQTT_LIB_TLS)
#if defined(CONFIG_MQTT_LIB_WEBSOCKET)
	client->transport.type = MQTT_TRANSPORT_SECURE_WEBSOCKET;
#else
	client->transport.type = MQTT_TRANSPORT_SECURE;
#endif

	struct mqtt_sec_config *tls_config = &client->transport.tls.config;

	tls_config->peer_verify = TLS_PEER_VERIFY_REQUIRED;
	tls_config->cipher_list = NULL;
	tls_config->sec_tag_list = m_sec_tags;
	tls_config->sec_tag_count = ARRAY_SIZE(m_sec_tags);
#if defined(CONFIG_MBEDTLS_X509_CRT_PARSE_C) || defined(CONFIG_NET_SOCKETS_OFFLOAD)
	tls_config->hostname = TLS_SNI_HOSTNAME;
#else
	tls_config->hostname = NULL;
#endif

#else
#if defined(CONFIG_MQTT_LIB_WEBSOCKET)
	client->transport.type = MQTT_TRANSPORT_NON_SECURE_WEBSOCKET;
#else
	client->transport.type = MQTT_TRANSPORT_NON_SECURE;
#endif
#endif

#if defined(CONFIG_MQTT_LIB_WEBSOCKET)
	client->transport.websocket.config.host = SERVER_ADDR;
	client->transport.websocket.config.url = "/mqtt";
	client->transport.websocket.config.tmp_buf = temp_ws_rx_buf;
	client->transport.websocket.config.tmp_buf_len =
						sizeof(temp_ws_rx_buf);
	client->transport.websocket.timeout = 5 * MSEC_PER_SEC;
#endif

#if defined(CONFIG_SOCKS)
	mqtt_client_set_proxy(client, &socks5_proxy,
			      socks5_proxy.sa_family == AF_INET ?
			      sizeof(struct sockaddr_in) :
			      sizeof(struct sockaddr_in6));
#endif
}

/* In this routine we block until the connected variable is 1 */
static int try_to_connect(struct mqtt_client *client)
{
	int rc, i = 0;

	while (i++ < APP_CONNECT_TRIES && !connected) {

		client_init(client);

		rc = mqtt_connect(client);
		if (rc != 0) {
			PRINT_RESULT("mqtt_connect", rc);
			k_sleep(K_MSEC(APP_SLEEP_MSECS));
			continue;
		}

		prepare_fds(client);

		if (wait(APP_CONNECT_TIMEOUT_MS)) {
			mqtt_input(client);
		}

		if (!connected) {
			mqtt_abort(client);
		}
	}

	if (connected) {
		return 0;
	}

	return -EINVAL;
}

static int process_mqtt_and_sleep(struct mqtt_client *client, int timeout)
{
	int64_t remaining = timeout;
	int64_t start_time = k_uptime_get();
	int rc;

	while (remaining > 0 && connected) {
		if (wait(remaining)) {
			rc = mqtt_input(client);
			if (rc != 0) {
				PRINT_RESULT("mqtt_input", rc);
				return rc;
			}
		}

		rc = mqtt_live(client);
		if (rc != 0 && rc != -EAGAIN) {
			PRINT_RESULT("mqtt_live", rc);
			return rc;
		} else if (rc == 0) {
			rc = mqtt_input(client);
			if (rc != 0) {
				PRINT_RESULT("mqtt_input", rc);
				return rc;
			}
		}

		remaining = timeout + start_time - k_uptime_get();
	}

	return 0;
}

#define SUCCESS_OR_EXIT(rc) { if (rc != 0) { return 1; } }
#define SUCCESS_OR_BREAK(rc) { if (rc != 0) { break; } }

static int publisher(void)
{
	int i, rc, r = 0;

	include_topic = true;
	aliases_enabled = false;

	LOG_INF("attempting to connect: ");
	rc = try_to_connect(&client_ctx);
	PRINT_RESULT("try_to_connect", rc);
	SUCCESS_OR_EXIT(rc);

	i = 0;
	while (i++ < CONFIG_NET_SAMPLE_APP_MAX_ITERATIONS && connected) {
		r = -1;

		rc = mqtt_ping(&client_ctx);
		PRINT_RESULT("mqtt_ping", rc);
		SUCCESS_OR_BREAK(rc);

		rc = process_mqtt_and_sleep(&client_ctx, APP_SLEEP_MSECS);
		SUCCESS_OR_BREAK(rc);

		rc = publish(&client_ctx, MQTT_QOS_0_AT_MOST_ONCE);
		PRINT_RESULT("mqtt_publish", rc);
		SUCCESS_OR_BREAK(rc);

		rc = process_mqtt_and_sleep(&client_ctx, APP_SLEEP_MSECS);
		SUCCESS_OR_BREAK(rc);

		rc = publish(&client_ctx, MQTT_QOS_1_AT_LEAST_ONCE);
		PRINT_RESULT("mqtt_publish", rc);
		SUCCESS_OR_BREAK(rc);

		rc = process_mqtt_and_sleep(&client_ctx, APP_SLEEP_MSECS);
		SUCCESS_OR_BREAK(rc);

		rc = publish(&client_ctx, MQTT_QOS_2_EXACTLY_ONCE);
		PRINT_RESULT("mqtt_publish", rc);
		SUCCESS_OR_BREAK(rc);

		rc = process_mqtt_and_sleep(&client_ctx, APP_SLEEP_MSECS);
		SUCCESS_OR_BREAK(rc);

		r = 0;
	}

	rc = mqtt_disconnect(&client_ctx, NULL);
	PRINT_RESULT("mqtt_disconnect", rc);

	LOG_INF("Bye!");

	return r;
}

static int start_app(void)
{
	int r = 0;
	int i = 0;

	if (!device_is_ready(mpu6050_dev)) {
		LOG_ERR("MPU6050 device is not ready");
		imu_is_ready = false;
	} else {
		LOG_INF("MPU6050 device is ready");
		imu_is_ready = true;
	}

	while (!CONFIG_NET_SAMPLE_APP_MAX_CONNECTIONS ||
	       i++ < CONFIG_NET_SAMPLE_APP_MAX_CONNECTIONS) {
		r = publisher();

		if (!CONFIG_NET_SAMPLE_APP_MAX_CONNECTIONS) {
			k_sleep(K_MSEC(5000));
		}
	}

	return r;
}

#if defined(CONFIG_USERSPACE)
#define STACK_SIZE 2048

#if defined(CONFIG_NET_TC_THREAD_COOPERATIVE)
#define THREAD_PRIORITY K_PRIO_COOP(CONFIG_NUM_COOP_PRIORITIES - 1)
#else
#define THREAD_PRIORITY K_PRIO_PREEMPT(8)
#endif

K_THREAD_DEFINE(app_thread, STACK_SIZE,
		start_app, NULL, NULL, NULL,
		THREAD_PRIORITY, K_USER, -1);

static K_HEAP_DEFINE(app_mem_pool, 1024 * 2);
#endif

int main(void)
{
	wait_for_network();

#if defined(CONFIG_MQTT_LIB_TLS)
	int rc;

	rc = tls_init();
	PRINT_RESULT("tls_init", rc);
#endif

#if defined(CONFIG_USERSPACE)
	int ret;

	struct k_mem_partition *parts[] = {
#if Z_LIBC_PARTITION_EXISTS
		&z_libc_partition,
#endif
		&app_partition
	};

	ret = k_mem_domain_init(&app_domain, ARRAY_SIZE(parts), parts);
	__ASSERT(ret == 0, "k_mem_domain_init() failed %d", ret);
	ARG_UNUSED(ret);

	k_mem_domain_add_thread(&app_domain, app_thread);
	k_thread_heap_assign(app_thread, &app_mem_pool);

	k_thread_start(app_thread);
	k_thread_join(app_thread, K_FOREVER);
#else
	exit(start_app());
#endif
	return 0;
}