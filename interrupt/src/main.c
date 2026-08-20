#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>

LOG_MODULE_REGISTER(app,LOG_LEVEL_INF);

#define LED_NODE DT_ALIAS(led)

#if  !DT_NODE_EXISTS(LED_NODE)
#error "led node does not exists"
#endif


struct gpio_callback cb;
static const struct gpio_dt_spec led =GPIO_DT_SPEC_GET(LED_NODE,gpios);
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(DT_ALIAS(sw0),gpios);

void work_handle(struct k_work *work)
{
    LOG_INF("ENTERED THE WORKING HANDLE");
    
    int value=gpio_pin_get_dt(&button);
    if(value == 1)
    {
        gpio_pin_toggle_dt(&led);
        LOG_INF("Button Pressed");
    }
}

K_WORK_DELAYABLE_DEFINE(work_struct,work_handle);

void isr_handle(const struct device *dev,struct gpio_callback *cb,uint32_t pins)
{
    k_work_reschedule(&work_struct,K_MSEC(500));
}

int main(void)
{
    
    if(!gpio_is_ready_dt(&led) || !gpio_is_ready_dt(&button))
    {
        return 0;
    }

    gpio_pin_configure_dt(&led,GPIO_OUTPUT_INACTIVE);

    gpio_pin_configure_dt(&button,GPIO_INPUT);

    gpio_pin_interrupt_configure_dt(&button,GPIO_INT_EDGE_TO_ACTIVE);

    gpio_init_callback(&cb,isr_handle,BIT(button.pin));

    gpio_add_callback(button.port,&cb);

    return 0;

    
}
    


