#include<zephyr/kernel.h>
#include<zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>

#define STACK_SIZE (1024)
#define COOP_PRIORITY (-2)
#define PREEMPT_PRIORITY (1)

#define LED DT_ALIAS(led)
#if !DT_NODE_EXISTS(LED)
#error "No led device found"
#endif

LOG_MODULE_REGISTER(app,LOG_LEVEL_INF);

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED,gpios);

void coop_thread(void* p1,void* p2,void* p3)
{
    LOG_INF("COOPERATIVE THREAD IS RUNNING.");
    k_msleep(3000);
    LOG_INF("COOPERATIVE THREAD TASK FINISHED.");
    k_msleep(3000);
}

void preempt_thread(void* p1,void* p2, void* p3)
{
    LOG_INF("PREEMPTIVE THREAD IS RUNNING.");
    while(1){
        gpio_pin_toggle_dt(&led);
        k_msleep(500);
    }
    

}

K_THREAD_DEFINE(coop,STACK_SIZE,coop_thread,NULL,NULL,NULL,COOP_PRIORITY,0,0);
K_THREAD_DEFINE(preempt,STACK_SIZE,preempt_thread,NULL,NULL,NULL,PREEMPT_PRIORITY,0,0);


int main(){
    LOG_INF("Hello world");
   
    if(!gpio_is_ready_dt(&led)){
        LOG_ERR("GPIO");
    }

    int ret =gpio_pin_configure_dt(&led,GPIO_OUTPUT_INACTIVE);

    if(ret < 0){
        LOG_ERR("GPIO");
    }

    return 0;
}






