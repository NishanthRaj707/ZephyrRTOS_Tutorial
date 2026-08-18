#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

/* Message data structure */
typedef struct {
    uint32_t id;
    uint8_t count;
} msg_data_t;

/* Define message queue: capacity of 5 messages, element size sizeof(msg_data_t), aligned to 4 bytes */
K_MSGQ_DEFINE(my_msgq, sizeof(msg_data_t), 5, 4);

/* Receiver thread entry point */
void get_handler(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    msg_data_t msg;

    while (1) {
        if (k_msgq_get(&my_msgq, &msg, K_FOREVER) == 0) {
            LOG_INF("Received Msg ID: %d, Count: %d", msg.id, msg.count);
        }
    }
}

/* Sender thread entry point */
void send_handler(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    msg_data_t msg;

    for (uint32_t i = 0; i < 1000; i++) {
        msg.id = i;
        msg.count = (uint8_t)(i % 256);

        if (k_msgq_put(&my_msgq, &msg, K_MSEC(1000)) == 0) {
            LOG_INF("Sent Msg ID: %d", msg.id);
        } else {
            LOG_WRN("Failed to send Msg ID: %d (Queue Full)", msg.id);
        }

        k_msleep(500);
    }
}

K_THREAD_DEFINE(thread_get, 2048, get_handler, NULL, NULL, NULL, 2, 0, 0);
K_THREAD_DEFINE(thread_send, 2048, send_handler, NULL, NULL, NULL, 1, 0, 0);

int main(void)
{
    LOG_INF("Message Queue Demo Started");
    return 0;
}