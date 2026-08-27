#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/fs/nvs.h>

#define NVS_PARTITION           storage_partition
#define NVS_PARTITION_DEVICE    FIXED_PARTITION_DEVICE(NVS_PARTITION)
#define NVS_PARTITION_OFFSET    FIXED_PARTITION_OFFSET(NVS_PARTITION)

#define RECORD_ID_BOOT_COUNT    1

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

static struct nvs_fs fs;

static int init_nvs(void)
{
    int ret;
    struct flash_pages_info info;

    fs.flash_device = NVS_PARTITION_DEVICE;
    if (!device_is_ready(fs.flash_device)) {
        LOG_ERR("Flash device not ready");
        return -ENODEV;
    }

    fs.offset = NVS_PARTITION_OFFSET;

    ret = flash_get_page_info_by_offs(fs.flash_device, fs.offset, &info);
    if (ret != 0) {
        LOG_ERR("Failed to get flash page info (Error: %d)", ret);
        return ret;
    }

    fs.sector_size = info.size;
    fs.sector_count = 3;

    LOG_INF("Mounting NVS: Offset 0x%lx, Sector Size %u Bytes, Sectors: %u",
            (long)fs.offset, fs.sector_size, fs.sector_count);

    ret = nvs_mount(&fs);
    if (ret != 0) {
        LOG_ERR("NVS mount failed (Error: %d)", ret);
        return ret;
    }

    return 0;
}

int main(void)
{
    int ret;
    uint32_t boot_count = 0;

    LOG_INF("BAVA IS RUNNING......");

    ret = init_nvs();
    if (ret != 0) {
        LOG_ERR("NVS initialization failed!");
        return 0;
    }

    ret = nvs_read(&fs, RECORD_ID_BOOT_COUNT, &boot_count, sizeof(boot_count));
    if (ret > 0) {
        LOG_INF("Recovered Boot Count from NVS: %u", boot_count);
    } else {
        LOG_WRN("No Boot Count record found. Initializing to 0 (First Boot).");
        boot_count = 0;
    }

    boot_count++;
    ret = nvs_write(&fs, RECORD_ID_BOOT_COUNT, &boot_count, sizeof(boot_count));
    if (ret >= 0) {
        LOG_INF("Updated Boot Count to %u in Flash (Bytes written: %d)", boot_count, ret);
    } else {
        LOG_ERR("Failed to write Boot Count (Error: %d)", ret);
    }

    return 0;
}