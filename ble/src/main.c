#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

LOG_MODULE_REGISTER(ble_app, LOG_LEVEL_INF);

#define SERVICE_UUID_VAL BT_UUID_128_ENCODE(0x0000ff00, 0x0000, 0x1000, 0x8000, 0x00805f9b34fb)
#define VALUE_UUID_VAL   BT_UUID_128_ENCODE(0x0000ff01, 0x0000, 0x1000, 0x8000, 0x00805f9b34fb)

static struct bt_uuid_128 service_uuid = BT_UUID_INIT_128(SERVICE_UUID_VAL);
static struct bt_uuid_128 value_uuid   = BT_UUID_INIT_128(VALUE_UUID_VAL);

static uint8_t sensor_data = 10;

static ssize_t on_read_value(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                            void *buf, uint16_t len, uint16_t offset)
{
    LOG_INF("Read request received, sending: %d", sensor_data);
    return bt_gatt_attr_read(conn, attr, buf, len, offset, &sensor_data, sizeof(sensor_data));
}

BT_GATT_SERVICE_DEFINE(custom_service,
    BT_GATT_PRIMARY_SERVICE(&service_uuid),
    BT_GATT_CHARACTERISTIC(&value_uuid.uuid,
                           BT_GATT_CHRC_READ,
                           BT_GATT_PERM_READ,
                           on_read_value,
                           NULL,
                           NULL)
);

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, "ESP32_Bava", sizeof("ESP32_Bava") - 1),
};

int main(void)
{
    int ret;

    LOG_INF("Booting BLE stack...");

    ret = bt_enable(NULL);
    if (ret != 0) {
        LOG_ERR("Failed to enable Bluetooth: %d", ret);
        return 0;
    }

    ret = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad), NULL, 0);
    if (ret != 0) {
        LOG_ERR("Failed to start advertising: %d", ret);
        return 0;
    }

    LOG_INF("Advertising as ESP32_Bava. Ready for connection.");

    while (1) {
        k_msleep(1000);
    }

    return 0;
}