#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/drivers/dma.h>

#define DMA DT_ALIAS(dma)
#if !DT_NODE_EXISTS(DMA)
#error "No dma device found"
#endif

#define BUFFER_SIZE 64
#define CHANNEL_ID 0


LOG_MODULE_REGISTER(dma_demo,LOG_LEVEL_INF);

K_SEM_DEFINE(dma_sem,0,1);

static const struct device *dma_dev = DEVICE_DT_GET(DMA);

static uint8_t src_buffer[BUFFER_SIZE] __aligned(4);
static uint8_t dest_buffer[BUFFER_SIZE] __aligned(4);

static void dma_callback(const struct device *dev,void* user_data,uint32_t channel_id,int status)
{
    if(status>=0){
        LOG_INF("Data transfer successful");
    }else{
        LOG_ERR("Data transfer failed %d",status);
    }

    k_sem_give(&dma_sem);
    
}


int main(void)
{
    if(!device_is_ready(dma_dev))
    {
        LOG_ERR("DMA is not ready ");
        return 0;
    }

    for(int i=0;i<BUFFER_SIZE;i++){
        src_buffer[i]=(uint8_t)i;
        dest_buffer[i]=0;
    }

    struct dma_block_config block_config={
        .source_address=(uint32_t)src_buffer,
        .dest_address=(uint32_t)dest_buffer,
        .block_size=BUFFER_SIZE,
        .source_addr_adj = DMA_ADDR_ADJ_INCREMENT,
        .dest_addr_adj   = DMA_ADDR_ADJ_INCREMENT,
        .next_block     = NULL,
    };

    struct dma_config config={
        .dma_channel         = CHANNEL_ID,
        .channel_direction   = MEMORY_TO_MEMORY,
        .source_data_size    = 1,
        .dest_data_size      = 1,
        .source_burst_length = 1,
        .dest_burst_length   = 1,
        .dma_callback        = dma_callback,
        .user_data           = NULL,
        .block_count         = 1,
        .head_block          = &block_cfg,
        .complete_callback_en = 1,
    };

    int ret = dma_config(dma_dev,CHANNEL_ID,&config);

    if(ret!=0){
        LOG_ERR("DMA config failed %d",ret);
        return 0;
    }
    
    ret =dma_start(dma_dev,CHANNEL_ID);
    
    if(ret!=0){
        LOG_ERR("DMA start failed %d",ret);
        return 0;
    }

    LOG_INF("DMA started. Waiting for ISR completion callback...");

    k_sem_take(&dma_sem,K_FOREVER);

    for(int i=0;i<BUFFER_SIZE;i++){
        if(src_buffer[i]!=dest_buffer[i]){
            LOG_ERR("Data mismatch at index %d: src=%d, dest=%d",i,src_buffer[i],dest_buffer[i]);
            return 0;
        }
    }
    
    LOG_INF("Data verification passed!");
    
    return 0;
}
