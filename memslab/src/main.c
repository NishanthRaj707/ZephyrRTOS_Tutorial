#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app,LOG_LEVEL_INF);

struct fifo_struct
{
    void* fifo_reserved;
    int id;
};

K_MEM_SLAB_DEFINE(var,sizeof(struct fifo_struct),10,4);
K_FIFO_DEFINE(my_fifo);

void producer(void* p1,void* p2,void* p3)
{
    ARG_UNUSED(p1);ARG_UNUSED(p2);ARG_UNUSED(p3);
    LOG_INF("ENTERING PRODUCER THREAD");
    
    struct fifo_struct *ptr;
    uint32_t seq = 0;

    while(1)
    {
        if(!k_mem_slab_alloc(&var,(void**)&ptr,K_FOREVER))
        {
            ptr->id=seq;
            k_fifo_put(&my_fifo,ptr);
            LOG_INF("PRODUCER PUSHED ID :%d",ptr->id);
            seq++;
        }
        k_msleep(1000);
    }
}

void consumer(void* p1,void* p2,void* p3)
{
    ARG_UNUSED(p1);ARG_UNUSED(p2);ARG_UNUSED(p3);
    LOG_INF("ENTERING CONSUMER THREAD");
    
    while(1){
    struct fifo_struct *rx = k_fifo_get(&my_fifo,K_FOREVER);

    if(rx != NULL)
    {
        LOG_INF("CONSUMER GOT ID :%d",rx->id);
        k_mem_slab_free(&var,(void *)&rx);
    }
}
}

K_THREAD_DEFINE(producer_thread,1024,producer,NULL,NULL,NULL,5,0,0);
K_THREAD_DEFINE(consumer_thread,1024,consumer,NULL,NULL,NULL,6,0,0);

int main(void){
    LOG_INF("MEMORY SLAB Tutorial with FiFo");
    return 0;
}