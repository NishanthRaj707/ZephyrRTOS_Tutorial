#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/spi.h>

#define SPI DT_ALIAS(sensor)

#if !DT_NODE_EXISTS(SPI)
#error "No SPI device found"
#endif

LOG_MODULE_REGISTER(app,LOG_LEVEL_INF);

static const struct spi_dt_spec bus = SPI_DT_SPEC_GET(SPI,
    SPI_WORD_SET(8) | SPI_OP_MODE_MASTER | SPI_TRANSFER_MSB, 
    0
);

K_MUTEX_DEFINE(spi_mutex);

int read_register(uint8_t id,uint8_t* var)
{
    int ret;

    uint8_t tx_cmd[2]={id|0x80,0x00};
    uint8_t rx_cmd[2]={0x00,0x00};

    struct spi_buf tx_buf={
        .buf=tx_cmd,
        .len=sizeof(tx_cmd)
    };

    struct spi_buf rx_buf={
        .buf=rx_cmd,
        .len=sizeof(rx_cmd)
    };

    struct spi_buf_set tx_set={
        .buffers=&tx_buf,
        .count=1,
    };

    struct spi_buf_set rx_set={
        .buffers=&rx_buf,
        .count=1,
    };

    k_mutex_lock(&spi_mutex,K_FOREVER);

    ret = spi_transceive_dt(&bus,&tx_set,&rx_set);

    k_mutex_unlock(&spi_mutex);

    if(ret==0)
    {
        *var=rx_cmd[1];
    }

    return ret;

}

int main(void)
{
    int ret;
    uint8_t data;

    LOG_INF("bAVA IS TRYING SPI");
    
    if(!spi_is_ready_dt(&bus))
    {
        return 0;
    }

    while(1)
    {
        ret=read_register(0x00,&data);

        LOG_INF("SENSOR VALUE:%d",data);
        
        if (ret == 0) {
            LOG_INF("SPI Transaction Success %d",ret);
        } else {
            LOG_ERR("SPI Transaction Failed ");
        }

        k_msleep(1000);
    
    }



    return 0;
}