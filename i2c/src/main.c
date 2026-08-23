#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>

#define TEMP DT_ALIAS(temperature)

#if !DT_NODE_EXISTS(TEMP)
#error "No node as temp found"
#endif

K_MUTEX_DEFINE(mutex);
LOG_MODULE_REGISTER(app,LOG_LEVEL_INF);

static const struct i2c_dt_spec sensor = I2C_DT_SPEC_GET(TEMP);

int read_register(uint8_t reg1,uint8_t* data)
{
    int ret;
    //Locking the mutex
    k_mutex_lock(&mutex,K_FOREVER);

    //Writing and retriving the data...
    ret=i2c_write_read_dt(&sensor,&reg1,1,data,1);
    
    //Unlocking the mutex
    k_mutex_unlock(&mutex);
    
    return ret;
}


int main(void)
{
    int ret;
    LOG_INF("I2C communication test...");
    
    if(!i2c_is_ready_dt(&sensor))
    {
        LOG_ERR("SENSOR FAILED");
        return 0;
    }

    LOG_INF("SENSOR ADDR -> 0x%x",sensor.addr);

    while(1)
    {
        uint8_t data;
    
        ret=read_register(0x00,&data);

        if(ret==0)
        {
            LOG_INF("DATA -> %d",data);
        }
        else{
            LOG_ERR("FAILED TO READ");
        }
        k_msleep(500);
    }
    return 0;
}