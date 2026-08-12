#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/gpio.h>

#define LED DT_ALIAS(led)

#if !DT_NODE_EXISTS(LED)
#error "No LED defined in the devicetree"
#endif

#define LABEL DT_PROP(LED,label)
#define PIN DT_GPIO_PIN(LED,gpios)
#define FLAG DT_GPIO_FLAGS(LED,gpios)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED,gpios);

int main(void)
{
    int ret;
    
    printk("\n===================================================\n");
    printk("       ZEPHYR RTOS: HARDWARE INSPECTOR & BLINK     \n");
    printk("===================================================\n\n");

    /* -------------------------------------------------------------
     * SECTION A: KCONFIG SYSTEM ENVIRONMENT INSPECTION
     * ------------------------------------------------------------- */
    printk("[KCONFIG] Architecture Target:\n");
    #if defined(CONFIG_ARM)
        printk("          -> ARM Cortex-M Series (e.g., STM32)\n");
    #elif defined(CONFIG_XTENSA)
        printk("          -> Xtensa Architecture (e.g., ESP32)\n");
    #elif defined(CONFIG_RISCV)
        printk("          -> RISC-V Architecture\n");
    #else
        printk("          -> Unknown/Other Architecture\n");
    #endif

    printk("[KCONFIG] Console Subsystem Status : ENABLED (%d)\n", IS_ENABLED(CONFIG_CONSOLE));
    printk("[KCONFIG] GPIO Driver Subsystem    : ENABLED (%d)\n", IS_ENABLED(CONFIG_GPIO));
    printk("[KCONFIG] System Tick Rate         : %d Hz\n", CONFIG_SYS_CLOCK_TICKS_PER_SEC);


    /* -------------------------------------------------------------
     * SECTION B: DEVICE TREE HARDWARE INSPECTION
     * ------------------------------------------------------------- */
    printk("[DT] LED Node Label                 : %s\n", LABEL);
    printk("[DT] GPIO Device                    : %s\n", led.port->name);
    printk("[DT] GPIO Pin Number               : %d\n", PIN);
    printk("[DT] GPIO Active Flag              : %d\n", FLAG);

    if(!gpio_is_ready_dt(&led))
    {
        return 0;
    }
    
    printk("LED device is ready\n");
    ret = gpio_pin_configure_dt(&led,GPIO_OUTPUT_INACTIVE);
    if(ret<0)
    {
        return 0;
    }
    
    while(1)
    {
        gpio_pin_toggle_dt(&led);
        k_msleep(1000);
    }
    
    return 0;
}