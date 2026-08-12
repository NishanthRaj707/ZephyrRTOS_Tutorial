#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>

#define STACK 1024
#define PRIORITY 3

#define LED DT_ALIAS(led)
#if !DT_NODE_EXISTS(LED)
#error "LED device not found "
#endif


LOG_MODULE_REGISTER(app,LOG_LEVEL_INF);

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED,gpios);

void thread_a(void* p1,void* p2,void* p3){
    while(1){
        LOG_INF("I AM THREAD A ----------------------------------");
        k_msleep(1000);
    }
}

void thread_b(void* p1,void* p2,void* p3){
    k_sched_lock();
    LOG_INF("i am THREAD b --------------------------------------------");
    gpio_pin_toggle_dt(&led);
    k_msleep(2000);
    LOG_INF("I AM EXIT");
    k_sched_unlock();
}

K_THREAD_DEFINE(thread1,STACK,thread_a,NULL,NULL,NULL,PRIORITY,0,0);
K_THREAD_DEFINE(thread2,STACK,thread_b,NULL,NULL,NULL,PRIORITY,0,0);


int main(void)
{
    if(!gpio_is_ready_dt(&led)){
        LOG_ERR("LED DEVICE NOT READY ");
        return 1;
    }
    
    int ret = gpio_pin_configure_dt(&led,GPIO_OUTPUT_INACTIVE);
    if(ret != 0)
    {
        LOG_ERR("Failed to configure LED : %d",ret);
    }
    
    return 0;
}