#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

#define LED_NODE DT_ALIAS(led)
#define STACK_SIZE 2048

#if !DT_NODE_EXISTS(LED_NODE)
#error "Devicetree alias led is not defined"
#endif

K_THREAD_STACK_DEFINE(stack_q, STACK_SIZE);
static struct k_work_q custom_work_q;

typedef struct {
    struct k_work work;
    uint8_t status;
} led_state;

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

void event_handler(struct k_work *work)
{
    led_state *state = CONTAINER_OF(work, led_state, work);

    if (state->status == 1) {
        gpio_pin_set_dt(&led, 1);
        LOG_INF("LED is ON");
    } else {
        gpio_pin_set_dt(&led, 0);
        LOG_INF("LED is OFF");
    }
}

led_state led_on = {
    .work = Z_WORK_INITIALIZER(event_handler),
    .status = 1,
};

led_state led_off = {
    .work = Z_WORK_INITIALIZER(event_handler),
    .status = 0,
};

int main(void)
{
    k_work_queue_init(&custom_work_q);

    k_work_queue_start(&custom_work_q, stack_q, K_THREAD_STACK_SIZEOF(stack_q), K_PRIO_COOP(1), NULL);

    LOG_INF("BAVA is starting...");

    if (!gpio_is_ready_dt(&led)) {
        LOG_ERR("LED GPIO device not ready");
        return 0;
    }

    int ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        LOG_ERR("Failed to configure LED GPIO pin: %d", ret);
        return 0;
    }

    while (1) {
        k_work_submit_to_queue(&custom_work_q, &led_on.work);
        k_msleep(1000);
        k_work_submit_to_queue(&custom_work_q, &led_off.work);
        k_msleep(1000);
    }
    return 0;
}

