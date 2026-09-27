#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_core.h>
#include <zephyr/net/net_mgmt.h>
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/net_context.h>
#include <string.h>

#define SSID "your ssid"
#define PASSWORD "your password"

K_SEM_DEFINE(wifi_connected, 0, 1);
K_SEM_DEFINE(ip_ready, 0, 1);

LOG_MODULE_REGISTER(wifi_station, LOG_LEVEL_INF);

static struct net_mgmt_event_callback wifi_cb;
static struct net_mgmt_event_callback ip_cb;

static void wifi_callback(struct net_mgmt_event_callback *cb, uint32_t event, struct net_if *iface)
{
    if (event == NET_EVENT_WIFI_CONNECT_RESULT) {
        LOG_INF(">> [Wi-Fi] Connected to AP successfully!");
        k_sem_give(&wifi_connected);
    } 
    else if (event == NET_EVENT_WIFI_DISCONNECT_RESULT) {
        LOG_WRN(">> [Wi-Fi] Disconnected from AP.");
    }
}

static void ip_callback(struct net_mgmt_event_callback *cb, uint32_t event, struct net_if *iface)
{
    if (event == NET_EVENT_IPV4_ADDR_ADD) {
        LOG_INF(">> [IPv4] IP Address assigned via DHCP!");
        k_sem_give(&ip_ready);
    }
}

static int connect_wifi(struct net_if *iface)
{
    struct wifi_connect_req_params config = {
        .ssid = (const uint8_t *)SSID,
        .ssid_length = strlen(SSID),
        .psk = (const uint8_t *)PASSWORD,
        .psk_length = strlen(PASSWORD),
        .channel = WIFI_CHANNEL_ANY,
        .security = WIFI_SECURITY_TYPE_PSK,
        .band = WIFI_FREQ_BAND_2_4_GHZ,
        .mfp = WIFI_MFP_OPTIONAL,
    };

    return net_mgmt(NET_REQUEST_WIFI_CONNECT, iface, &config, sizeof(config));
}

int main(void)
{
    LOG_INF("Starting Wi-Fi Station Demo...");

    net_mgmt_init_event_callback(&wifi_cb, wifi_callback, 
                                 NET_EVENT_WIFI_CONNECT_RESULT | NET_EVENT_WIFI_DISCONNECT_RESULT);
    net_mgmt_add_event_callback(&wifi_cb);

    net_mgmt_init_event_callback(&ip_cb, ip_callback, NET_EVENT_IPV4_ADDR_ADD);
    net_mgmt_add_event_callback(&ip_cb);

    /* Get the default network interface pointer */
    struct net_if *iface = net_if_get_default();
    if (!iface) {
        LOG_ERR("No default network interface found!");
        return 0;
    }

    int ret = connect_wifi(iface);
    if (ret != 0) {
        LOG_ERR("net_mgmt connection request failed: %d", ret);
        return 0;
    }

    LOG_INF("Waiting for Wi-Fi association...");
    if (k_sem_take(&wifi_connected, K_SECONDS(20)) != 0) {
        LOG_ERR("Wi-Fi connection timed out!");
        return 0;
    }

    LOG_INF("Associated with AP. Requesting DHCP lease...");
    if (k_sem_take(&ip_ready, K_SECONDS(20)) != 0) {
        LOG_ERR("DHCP address acquisition timed out!");
        return 0;
    }

    LOG_INF(">> Network is fully online and ready for socket communication!");

    while (1) {
        k_msleep(10000);
    }

    return 0;
}