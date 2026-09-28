#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_core.h>
#include <zephyr/net/net_mgmt.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/wifi_mgmt.h>

LOG_MODULE_REGISTER(app,LOG_LEVEL_INF);

#define SSID "Your ssid"
#define PASS "Your password"

#define TARGET "tcpbin.com"
#define PORT "4242"

static struct net_mgmt_event_callback wifi_cb;
static struct net_mgmt_event_callback ip_cb;

K_SEM_DEFINE(wifi, 0, 1);
K_SEM_DEFINE(ip, 0, 1);


static void wifi_callback(struct net_mgmt_event_callback *cb, uint32_t event, struct net_if *iface)
{
    if(event == NET_EVENT_WIFI_CONNECT_RESULT) {
        LOG_INF(">> [Wi-Fi] Connected to AP successfully!");
        k_sem_give(&wifi);
    }
    else if (event == NET_EVENT_WIFI_DISCONNECT_RESULT) {
        LOG_WRN(">> [Wi-Fi] Disconnected from AP.");
    }
}

static void ip_callback(struct net_mgmt_event_callback *cb,uint32_t event,struct net_if* iface)
{
    if(event==NET_EVENT_IPV4_ADDR_ADD)
    {
        LOG_INF("Assigned IP");
        k_sem_give(&ip);
    }
}

static int tcp_function(void)
{
    int sock=-1;
    int ret;

    struct zsock_addrinfo hints={
        .ai_family=AF_INET,
        .ai_socktype=SOCK_STREAM,
        .ai_protocol=IPPROTO_TCP
    };

    struct zsock_addrinfo* res=NULL;

    ret=getaddrinfo(TARGET)


}

int main(void)
{
    LOG_INF("Starting socket demo");

    net_mgmt_init_event_callback(&wifi_cb,wifi_callback,NET_EVENT_WIFI_CONNECT_RESULT|NET_EVENT_WIFI_DISCONNECT_RESULT);
    net_mgmt_add_event_callback(&wifi_cb);

    net_mgmt_init_event_callback(&ip_cb,ip_callback,NET_EVENT_IPV4_ADDR_ADD);
    net_mgmt_add_event_callback(&ip_cb);

    struct net_if* iface = net_if_get_default();
    if(!iface) {
        LOG_ERR("No default net");
    }
    
    struct wifi_connect_req_params params={
        .ssid=(const uint8_t*)SSID,
        .ssid_length=strlen(SSID),
        .psk=(const uint8_t*)PASS,
        .psk_length=strlen(PASS),
        .channel=WIFI_CHANNEL_ANY,
        .security=WIFI_SECURITY_TYPE_PSK,
        .band=WIFI_FREQ_BAND_2_4_GHZ,
        .mfp=WIFI_MFP_OPTIONAL
    };

    if(net_mgmt(NET_REQUEST_WIFI_CONNECT,iface,&params,sizeof(params))!=0)
    {
        LOG_ERR("Failed to connect to WiFi");
        return 0;
    }

    if(k_sem_take(&wifi,K_SECONDS(20))!=0)
    {
        LOG_ERR("Wifi connection timed out");
        return 0;
    }

    if(k_sem_take(&ip,K_SECONDS(20))!=0)
    {
        LOG_ERR("IP acquisition timed out");
        return 0;
    }
    
    LOG_INF("Network is ready for socket operations");

    tcp_function()

    while(1)
    {
        k_msleep(5000);
    }
    return 0;
}