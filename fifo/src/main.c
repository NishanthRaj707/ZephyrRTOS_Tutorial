#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app,LOG_LEVEL_INF);

#define BUFFER 4
#define STACK 1024


K_FIFO_DEFINE(my_fifo);

struct my_struct{
    void* fifo_reserved;
    int id;
};

static struct my_struct buffer_array[BUFFER];


void producer(void* p1,void* p2,void* p3)
{
    ARG_UNUSED(p1);ARG_UNUSED(p2);ARG_UNUSED(p3);

    uint32_t seq = 0;
    LOG_INF("ENTERING PRODUCER THREAD");
    
    while(1)
    {
        struct my_struct *ptr = &buffer_array[seq % BUFFER];
        seq++;
        ptr->id=seq;

        LOG_INF("PRODUCER PUSHED ID :%d",ptr->id);
        k_fifo_put(&my_fifo,ptr);
        k_msleep(20000);
    }
}

void consumer(void* p1,void* p2,void* p3)
{
    ARG_UNUSED(p1);ARG_UNUSED(p2);ARG_UNUSED(p3);

    LOG_INF("ENTERING CONSUMER THREAD");

    while(1)
    {
        struct my_struct *rx_ptr = k_fifo_get(&my_fifo, K_FOREVER);

        LOG_INF("CONSUMER RECEIVED ID :%d", rx_ptr->id);

    }
}
K_THREAD_DEFINE(producer_thread, STACK, producer, NULL, NULL, NULL, 1, 0, 0);
K_THREAD_DEFINE(consumer_thread, STACK, consumer, NULL, NULL, NULL, 2, 0, 0);

int main(void)
{
    LOG_INF("BAVA.....");
    return 0;
}