#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <drivers/bava_counter.h>

LOG_MODULE_REGISTER(main_app,LOG_LEVEL_INF);

static const struct device* counter=DEVICE_DT_GET(DT_NODELABEL(fast_counter));

int main(void)
{
    if(!device_is_ready(counter))
    {
        LOG_ERR("DEVICE NOT READY");
        return 0;
    }

    LOG_INF("STARTED........");
    int* val;
    while(1)
    {
        ret=bava_counter_get(counter,val);
        LOG_INF("COUNTER VALUE:%d",*val);
        k_msleep(1000);
    }

    return 1;
}