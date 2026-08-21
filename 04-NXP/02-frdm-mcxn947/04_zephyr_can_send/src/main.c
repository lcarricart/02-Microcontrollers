/*------------------------------------------------------------------------------------------------------------------
 * Objective: send a CAN frame every time the button is pressed.
 -------------------------------------------------------------------------------------------------------------------
 * Conclusions: 
 -------------------------------------------------------------------------------------------------------------------
 * Author: Luciano Carricart, https://github.com/lcarricart/
 * Status: Information Engineering student, HAW Hamburg, Germany.
 ------------------------------------------------------------------------------------------------------------------*/

#include "zephyr/kernel/thread_stack.h"
#include "zephyr/sys/__assert.h"
#include "zephyr/toolchain.h"
#include <stdint.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/can.h>

#define LED_NODE DT_ALIAS(led0)
#define BUTTON_NODE DT_ALIAS(sw0)

#define CAN_NODE DT_NODELABEL(flexcan0)

#define CAN_TX_PRIORITY 5
K_THREAD_STACK_DEFINE(can_tx_stack, 1024);

K_MSGQ_DEFINE(can_tx_queue, sizeof(struct can_frame), 4, 4);

/*-----------------------------------------------------------------------------------------
    Variables
  -----------------------------------------------------------------------------------------*/
static struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);
static struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BUTTON_NODE, gpios);

static struct gpio_callback button_cb;

/* CAN variables */
const struct device* can = DEVICE_DT_GET(CAN_NODE);
static volatile uint32_t tx_dropped_frames = 0;
struct can_filter filter = 
{
    .id = 0x123,
    .mask = CAN_STD_ID_MASK,
    .flags = 0 /* standard or extended frame format selection */
};
struct can_frame tx_frame = 
{
    .id = 0x123,
    .dlc = 4,
    .flags = 0, /* standard format */
    .data = {0x11, 0x22, 0x33, 0x44}
};

/*-----------------------------------------------------------------------------------------
    Prototypes
  -----------------------------------------------------------------------------------------*/
static void button_pressed(const struct device* dev, struct gpio_callback* cb, uint32_t pins);
static void can_tx_thread(void *p1, void *p2, void *p3);

/*-----------------------------------------------------------------------------------------
    Functions
  -----------------------------------------------------------------------------------------*/
/* In Zephyr, one does not typically call a global scheduler-start function */
int main(void)
{
    printk("Application start!");

    /* Devices readiness */
    if (!gpio_is_ready_dt(&led) || !gpio_is_ready_dt(&button) || !device_is_ready(can)) {
        printk("A device is not ready \n");
        return 0;
    }
    printk("Devices ready");

    /* Devices configuration */
    gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&button, GPIO_INPUT);

    /* CAN Configuration */
    int ret;
    ret = can_set_mode(can, CAN_MODE_NORMAL);
    __ASSERT(ret == 0, "CAN normal configuration failure");
    ret = can_set_bitrate(can, 50000);
    __ASSERT(ret == 0, "CAN bitrate configuration failure");
    ret = can_start(can);
    __ASSERT(ret == 0, "CAN driver start failure");

    printk("Configurations ready");

    /* Interrupts (no CAN) */
    gpio_init_callback(&button_cb, button_pressed, BIT(button.pin)); /* BIT(...) can be replace by (1U << 23U) but defeating the whole purpose of portability */
    gpio_add_callback_dt(&button, &button_cb);
    gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE); /* GPIO_INT_EDGE_FALLING is nxp mcxn-specific, because of it's ACTIVE_LOW characteristics. Also defeats portability */

    /* Main thread creates threads */
    static struct k_thread can_tx_thread_data;
    k_thread_create(&can_tx_thread_data, can_tx_stack, K_THREAD_STACK_SIZEOF(can_tx_stack), can_tx_thread, NULL, NULL, NULL, CAN_TX_PRIORITY, 0, K_NO_WAIT);

    while (1)
    {
        return 0; /* Main thread fininishes, it's only work was to initialize and delegate */
    }
}

/*-----------------------------------------------------------------------------------------
    Other Threads
  -----------------------------------------------------------------------------------------*/
static void can_tx_thread(void *p1, void *p2, void *p3)
{
    int ret;
    struct can_frame frame;

    while(1)
    {
        k_msgq_get(&can_tx_queue, &frame, K_FOREVER);

        ret = can_send(can, &frame, K_FOREVER, NULL, NULL);
        __ASSERT(ret == 0, "CAN transmission failure");
    }
}

/*-----------------------------------------------------------------------------------------
    Callbacks
  -----------------------------------------------------------------------------------------*/
/* The callback must match Zephyr’s callback type exactly, or else gpio_init_callback() can't be called. This is why "const" needs to stay */
static void button_pressed(const struct device* dev, struct gpio_callback* io_cb, uint32_t pins)
{
    ARG_UNUSED(dev);
    ARG_UNUSED(io_cb);
    ARG_UNUSED(pins);
    gpio_pin_toggle_dt(&led);

    /* With the field callback == NULL, can_send() blocks until the frame is actually transmitted or an error occurs
    int ret = can_send(can, &tx_frame, K_FOREVER, NULL, NULL);
    __ASSERT(ret == 0, "CAN transmission failure");
    
    We need an asynchronous send as well. The way it works is send the frame to a queue, and let a thread handle the can_send() */

    int ret = k_msgq_put(&can_tx_queue, &tx_frame, K_NO_WAIT);

    if (ret != 0)
    {
        tx_dropped_frames++;
    }
}