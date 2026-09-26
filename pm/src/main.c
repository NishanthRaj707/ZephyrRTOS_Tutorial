#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/pm/pm.h>
#include <zephyr/sys/poweroff.h>
#include <esp_sleep.h>
#include <driver/gpio.h>

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

static RTC_DATA_ATTR uint32_t wakeup_counter = 0;

int main(void)
{
    wakeup_counter++;

    LOG_INF("woken up %d times", wakeup_counter);

    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();

    LOG_INF("wakeup cause %d", cause);

    switch (cause) {
    case ESP_SLEEP_WAKEUP_TIMER:
        LOG_INF("wakeup cause timer");
        break;
    case ESP_SLEEP_WAKEUP_EXT0:
        LOG_INF("wakeup cause external pin");
        break;
    default:
        LOG_INF("wakeup cause unknown");
        break;
    }

    k_msleep(1000);

    esp_sleep_enable_timer_wakeup(5000000);

    esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0);

    sys_poweroff();

    return 0;
}
