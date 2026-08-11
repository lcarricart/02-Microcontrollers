#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* In Zephyr, startup code, architecture init, the kernel, and configured device init run before application main(). The latter executes as the main thread. One does not typically call a global scheduler-start function in this RTOS */
int main(void)
{
    while (1)
    {
        k_sleep(K_SECONDS(1));
    }
    return 0;
}