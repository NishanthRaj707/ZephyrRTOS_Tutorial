#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

#define LED DT_ALIAS(led)

#if !DT_NODE_EXISTS(LED)
#error "led not found"
#endif

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED,gpios);

LOG_MODULE_REGISTER(app,LOG_LEVEL_INF);

void led_on_handler(struct k_work* work){
    gpio_pin_set_dt(&led,1);
    LOG_INF("LED ON");
}

void led_off_handler(struct k_work* work){
    gpio_pin_set_dt(&led,0);
    LOG_INF("LED OFF");
}

K_WORK_DEFINE(led_on,led_on_handler);
K_WORK_DEFINE(led_off,led_off_handler);

int main(void){
    
    LOG_INF("BAVA IS STARTING ..........");
    
    if(!gpio_is_ready_dt(&led)){
        LOG_ERR("LED not ready");
        return 0;
    }

    int ret = gpio_pin_configure_dt(&led,GPIO_OUTPUT_INACTIVE);
    if(ret<0){
        LOG_ERR("LED configuration failed");
        return 0;
    }

    while(1){
        k_work_submit(&led_on);
        k_msleep(1000);
        k_work_submit(&led_off);
        k_msleep(1000);
    }
    return 0;
}