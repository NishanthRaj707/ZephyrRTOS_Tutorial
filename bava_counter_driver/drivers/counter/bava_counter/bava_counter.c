#define DT_DRV_COMPAT bava_counter

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <drivers/bava_counter.h>

LOG_MODULE_REGISTER(bava_counter_driver, LOG_LEVEL_INF);

struct bava_counter_config {
    int start_offset;
    int step_size;
};

struct bava_counter_data {
    int counter;
    struct k_mutex lock;
};

static int bava_get(const struct device *dev, int *val)
{
    const struct bava_counter_config *config = dev->config;
    struct bava_counter_data *data = dev->data;

    if (val == NULL) {
        return -EINVAL;
    }

    k_mutex_lock(&data->lock, K_FOREVER);

    *val = data->counter;
    data->counter += config->step_size;

    k_mutex_unlock(&data->lock);

    return 0;
}

static int bava_reset(const struct device *dev)
{
    const struct bava_counter_config *config = dev->config;
    struct bava_counter_data *data = dev->data;

    k_mutex_lock(&data->lock, K_FOREVER);

    data->counter = config->start_offset;

    k_mutex_unlock(&data->lock);

    LOG_INF("RESETTING ENTIRE SYSTEM");

    return 0;
}

static const struct bava_counter_api bava_api = {
    .get = bava_get,
    .reset = bava_reset,
};

static int bava_counter_init(const struct device *dev)
{
    const struct bava_counter_config *config = dev->config;
    struct bava_counter_data *data = dev->data;

    k_mutex_init(&data->lock);

    data->counter = config->start_offset;

    return 0;
}

#define BAVA_COUNTER_DEFINE(inst) \
    static const struct bava_counter_config config_##inst = { \
        .start_offset = DT_INST_PROP(inst, start_offset), \
        .step_size = DT_INST_PROP_OR(inst, step_size, 1), \
    }; \
    static struct bava_counter_data data_##inst; \
    DEVICE_DT_INST_DEFINE( \
        inst, \
        bava_counter_init, \
        NULL, \
        &data_##inst, \
        &config_##inst, \
        POST_KERNEL, \
        CONFIG_KERNEL_INIT_PRIORITY_DEVICE, \
        &bava_api \
    );

DT_INST_FOREACH_STATUS_OKAY(BAVA_COUNTER_DEFINE)
