#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>

#define LED_NODE DT_ALIAS(led0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

/* In Zephyr, startup code, architecture init, the kernel, and configured device init run before application main(). The latter executes as the main thread. One does not typically call a global scheduler-start function in this RTOS */
int main(void)
{
    if (!gpio_is_ready_dt(&led)) {
        return 0; /* It does not need to wait like a while(!ready) would. It just checks if the device is initialized and usable */
    }

    gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);

    while (1)
    {
        /* This implies a lot of abstraction honestly */
        gpio_pin_toggle_dt(&led);

        /* Another way of doing this is setting the physical electrical level of the pin manually */
        // gpio_pin_set_raw(led.port, led.pin, 1);
        // gpio_pin_set_raw(led.port, led.pin, 0);

        k_sleep(K_SECONDS(1));
    }
    return 0;
}
