/*******************************************************************************************************************
 * Objective: Learn Zephyr project structure and festures by means of a pushbutton application, triggering an interrupt.
 *******************************************************************************************************************
 * Conclusions: I run with a curious issue while debugging. Let's document also the interesting debug process:
    1- Open a debug server using CLI (west debugserver -d build -r linkserver)
    2- Download the VSC extension Cortex Debug, by marus25
    3- Create a launch.json with the required configuration and paths
    4- Press "Run and Debug"
 
   During the debugging session, I tried to include a breakpoint in my callback via VSC GUI, but the callback seems
   to have gotten optimized and inlined... Therefore, the only reliable way to put this callback was through
   Breakpoints > Function Name.
 *******************************************************************************************************************
 * Author: Luciano Carricart, https://github.com/lcarricart/
 * Status: Information Engineering student, HAW Hamburg, Germany.
 *******************************************************************************************************************/

#include <stdint.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>

#define LED_NODE DT_ALIAS(led0)
#define BUTTON_NODE DT_ALIAS(sw0)

static struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);
static struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BUTTON_NODE, gpios);

static struct gpio_callback button_cb;

/*========================================================================================
    Prototypes
  ========================================================================================*/
static void button_pressed(const struct device* dev, struct gpio_callback* cb, uint32_t pins);

/*========================================================================================
    Functions
  ========================================================================================*/

/* In Zephyr, one does not typically call a global scheduler-start function */
int main(void)
{
    /* Devices readiness */
    if (!gpio_is_ready_dt(&led) || !gpio_is_ready_dt(&button)) {
        return 0;
    }

    gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&button, GPIO_INPUT);

    /* Interrupt */
    gpio_init_callback(&button_cb, button_pressed, (1U << 23U)); /* hardcoded mask is worse than BIT(button.pin), always functional. Board definition may change! */
    gpio_add_callback_dt(&button, &button_cb);
    gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_FALLING);

    while (1)
    {
         k_sleep(K_FOREVER); /* Standard */
         //k_cpu_idle();     /* Alternative*/
         //__WFI();          /* ARM specific, non-conventional in Zephyr */
    }
    return 0;
}

/* The callback must match Zephyr’s callback type exactly, or else gpio_init_callback() can't be called. This is why "const" needs to stay */
static void button_pressed(const struct device* dev, struct gpio_callback* cb, uint32_t pins)
{
    gpio_pin_toggle_dt(&led);
}