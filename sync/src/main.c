#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>

#define LED DT_ALIAS(led)

#if !DT_NODE_EXISTS(LED)
#error "led not found in devicetree"
#endif 

#define STACK_SIZE (1024)
#define COOP_PRIORITY (-2)
#define PREEMPT (1)

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED, gpios);

K_MUTEX_DEFINE(sync_mutex);
K_SEM_DEFINE(sync_sem, 0, 1);

void preempt1_entry(void* p1, void* p2, void* p3) {
    while (1) {
        if (k_sem_take(&sync_sem, K_FOREVER) == 0) {
            LOG_INF("RUNNING THE PREEMPT 1");
            gpio_pin_toggle_dt(&led);
        }
    }
}

void coop_entry(void* p1, void* p2, void* p3) {
    while (1) {
        if (k_mutex_lock(&sync_mutex, K_FOREVER) == 0) {
            LOG_INF("RUNNING THE COOP");
            LOG_INF("TAKEN THE MUTEX");

            gpio_pin_set_dt(&led, 1);
            k_msleep(3000);

            gpio_pin_set_dt(&led, 0);
            k_msleep(2000);

            LOG_INF("GIVING THE SEMPHORE");
            k_sem_give(&sync_sem);

            k_mutex_unlock(&sync_mutex);
            k_msleep(10);
        }
    }
}

K_THREAD_DEFINE(preempt1_thread, STACK_SIZE, preempt1_entry, NULL, NULL, NULL, PREEMPT, 0, 100);
K_THREAD_DEFINE(coop_thread, STACK_SIZE, coop_entry, NULL, NULL, NULL, COOP_PRIORITY, 0, 100);

int main(void) {
    int ret;

    if (!gpio_is_ready_dt(&led)) {
        LOG_ERR("Error: GPIO device for LED is not ready");
        return 0;
    }

    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        LOG_ERR("Error: Failed to configure GPIO LED pin (err %d)", ret);
        return 0;
    }

    LOG_INF("GPIO LED initialized successfully");
    return 0;
}


