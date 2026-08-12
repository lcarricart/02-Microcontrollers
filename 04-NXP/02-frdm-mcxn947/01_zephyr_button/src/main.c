/*******************************************************************************************************************
 * Objective: Learn Zephyr project structure and festures by means of a pushbutton application.
 *******************************************************************************************************************
 * Author: Luciano Carricart, https://github.com/lcarricart/
 * Status: Information Engineering student, HAW Hamburg, Germany.
 *******************************************************************************************************************/

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>

#define LED_NODE DT_ALIAS(led0)
#define BUTTON_NODE DT_ALIAS(sw0)

static struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);
static struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BUTTON_NODE, gpios);

/* In Zephyr, startup code, architecture init, the kernel, and configured device init run before application main(). The latter executes as the main thread. One does not typically call a global scheduler-start function in this RTOS */
int main(void)
{
    int button_state;
    int prev_button_state = 0;

    if (!gpio_is_ready_dt(&led) || !gpio_is_ready_dt(&button)) {
        return 0; /* It does not need to wait like a while(!ready) would. It just checks if the device is initialized and usable */
    }

    gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&button, GPIO_INPUT);

    while (1)
    {
        button_state = gpio_pin_get_dt(&button);

        if ((button_state == 1) && (prev_button_state != button_state))
        {
            /* This implies a lot of abstraction honestly. Else, gpio_pin_set_raw(led.port, led.pin, 1); */
            gpio_pin_toggle_dt(&led);
        }

        prev_button_state = button_state;

        k_sleep(K_MSEC(20));
    }
    return 0;
}