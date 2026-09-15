#define DT_DRV_COMPACT bava_counter

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <drivers/counter.h>

LOG_MODULE_REGISTER(app,CONFIG_LEVEL_INF);

struct bava_counter_config{
    int start_offset;
    int step_size;
};

struct bava_counter_data{
    int counter;
    struct k_mutex lock;
}