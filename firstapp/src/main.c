#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define LED DT_ALIAS(led)

#if !DT_NODE_EXISTS(LED)
#error "Unsupported board: led0 device tree alias is not defined"
#endif

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED,gpios);

int main(void)
{
    if(!gpio_is_ready_dt(&led))
    {
        return 0;
    }

    int temp = gpio_pin_configure_dt(&led,GPIO_OUTPUT_INACTIVE);
    if(temp<0)
    {
        return 0;
    }

    while(1)
    {
        gpio_pin_toggle_dt(&led);
        k_msleep(500);

    }
}

