#ifndef ZEPHYR_INCLUDE_DRIVERS_BAVA_COUNTER_H_
#define ZEPHYR_INCLUDE_DRIVERS_BAVA_COUNTER_H_

#include <zephyr/device.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int(*bava_counter_get_t)(const struct device* dev,int* val);
typedef int(*bava_counter_reset_t)(const struct device* dev);


struct bava_counter_api
{
    bava_counter_get_t get;
    bava_counter_reset_t reset;
};

static inline int bava_counter_get(const struct device* dev,int* val)
{
    const struct bava_counter_api *api;

    if(!device_is_ready(dev))
    {
        return -ENODEV;
    }
    
    *api=(const struct bava_counter_api*)dev->api;

    if(api->get == NULL)
    {
        return -ENOSYS;
    }

    return api->get(dev,val);

}

static inline int bava_counter_reset(const struct device* dev)
{
    const struct bava_counter_api *api;

    if(!device_is_ready(dev))
    {
        return -ENODEV;
    }

    *api=(const struct bava_counter_api*)dev->api;

    if(api->reset ==  NULL)
    {
        return -ENOSYS;
    }

    return api->reset(dev);

}

#ifdef __cplusplus
}
#endif