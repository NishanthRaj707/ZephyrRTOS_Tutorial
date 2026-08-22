#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/gpio.h>
#include <strings.h>
#include <ctype.h>

#define LED_NODE DT_ALIAS(led)
#define UART_NODE DT_ALIAS(commuart)

#if !DT_NODE_EXISTS(LED_NODE)
#error "LED node alias not found"
#endif

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

K_SEM_DEFINE(rx_sem, 0, 1);
K_RING_BUF_DECLARE(rx_buf, 256);

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);
static const struct device *const uart = DEVICE_DT_GET(UART_NODE);

static void uart_callback(const struct device *dev, void *user_data)
{
    if (!uart_irq_update(dev)) {
        return;
    }

    if (uart_irq_rx_ready(dev)) {
        uint8_t byte;

        while (uart_fifo_read(dev, &byte, 1) > 0) {
            ring_buf_put(&rx_buf, &byte, 1);

            if (byte == '\n') {
                k_sem_give(&rx_sem);
            }
        }
    }
}

static void uart_send_string(const struct device *dev, const char *str)
{
    while (*str) {
        uart_poll_out(dev, *str++);
    }
}

void producer_thread(void *p1, void *p2, void *p3)
{
    char temp[256];
    uint8_t idx = 0;

    LOG_INF("PARSER STARTED");

    while (1) {
        k_sem_take(&rx_sem, K_FOREVER);

        while (ring_buf_get(&rx_buf, (uint8_t *)&temp[idx], 1) > 0) {
            if (temp[idx] == '\n') {
                /* Remove trailing '\r' if present */
                if (idx > 0 && temp[idx - 1] == '\r') {
                    temp[idx - 1] = '\0';
                } else {
                    temp[idx] = '\0';
                }

                if (strlen(temp) > 0) {
                    if (strcasecmp(temp, "LED ON") == 0) {
                        gpio_pin_set_dt(&led, 1);
                        uart_send_string(uart, "[ACK] LED is now ON\r\n");
                    } 
                    else if (strcasecmp(temp, "LED OFF") == 0) {
                        gpio_pin_set_dt(&led, 0);
                        uart_send_string(uart, "[ACK] LED is now OFF\r\n");
                    } 
                    else if (strcasecmp(temp, "STATUS") == 0) {
                        int state = gpio_pin_get_dt(&led);
                        uart_send_string(uart, state ? "[STATUS] LED State: ACTIVE\r\n" 
                                                     : "[STATUS] LED State: INACTIVE\r\n");
                    } 
                    else {
                        uart_send_string(uart, "[ERR] Unknown Command! Use: LED ON | LED OFF | STATUS\r\n");
                    }
                }

                uart_send_string(uart, "> ");
                idx = 0;
            } else {
                if (idx < 254) {
                    idx++;
                } else {
                    idx = 0;
                    uart_send_string(uart, "\r\n[ERR] Buffer overflow\r\n> ");
                }
            }
        }
    }
}

K_THREAD_DEFINE(producer, 1024, producer_thread, NULL, NULL, NULL, 5, 0, 0);

int main(void)
{
    if (!gpio_is_ready_dt(&led) || !device_is_ready(uart)) {
        LOG_ERR("Hardware initialization failed");
        return -1;
    }

    gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);

    uart_irq_callback_set(uart, uart_callback);
    uart_irq_rx_enable(uart);

    uart_send_string(uart, "[init] LED Parser Ready\r\n> ");

    return 0;
}