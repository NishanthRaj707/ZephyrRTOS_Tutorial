# Module 1: `firstapp` - Basic Application & GPIO Control

This introductory module demonstrates how to write a basic application in **Zephyr RTOS**, configure hardware using **Devicetree Overlays (`app.overlay`)**, enable kernel drivers using **Kconfig (`prj.conf`)**, and perform periodic hardware toggling (LED Blinky) using Zephyr's GPIO API and Kernel Delay functions.

---

## 🔑 Key Concepts Covered

1. **Devicetree Overlay (`app.overlay`)**:
   - Defining custom hardware nodes without modifying the base board devicetree.
   - Creating a `gpio-leds` node linked to **GPIO Bank B, Pin 2** (`<&gpiob 2 GPIO_ACTIVE_HIGH>`).
   - Defining a hardware alias (`led = &internalled`) for portability in C code.
2. **Kconfig Configuration (`prj.conf`)**:
   - Enabling the GPIO subsystem globally via `CONFIG_GPIO=y`.
3. **Zephyr GPIO API & Devicetree Macros**:
   - `DT_ALIAS(led)`: Fetching devicetree node identifier by alias.
   - `GPIO_DT_SPEC_GET(node, gpios)`: Extracting GPIO specification structure at compile time.
   - `gpio_is_ready_dt()`: Checking if the underlying hardware controller driver is ready.
   - `gpio_pin_configure_dt()`: Initializing the pin direction (Output Inactive).
   - `gpio_pin_toggle_dt()`: Toggling the pin output state.
   - `k_msleep(500)`: Putting the current thread to sleep for 500 milliseconds.

---

## 📁 File Structure

- [CMakeLists.txt](file:///home/bava/Desktop/ZephyrRTOS_Tutorial/firstapp/CMakeLists.txt): Build rules registering `src/main.c`.
- [prj.conf](file:///home/bava/Desktop/ZephyrRTOS_Tutorial/firstapp/prj.conf): Enables `CONFIG_GPIO=y`.
- [app.overlay](file:///home/bava/Desktop/ZephyrRTOS_Tutorial/firstapp/app.overlay): Overlays board hardware definitions for LED pin mapping.
- [src/main.c](file:///home/bava/Desktop/ZephyrRTOS_Tutorial/firstapp/src/main.c): Main application loop toggling the LED.

---

## 📝 Code Walkthrough

```c
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

// 1. Fetch Node Identifier from Devicetree Alias 'led'
#define LED DT_ALIAS(led)

#if !DT_NODE_EXISTS(LED)
#error "Unsupported board: led alias is not defined"
#endif

// 2. Obtain GPIO Spec Structure from Devicetree Node at compile time
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED, gpios);

int main(void)
{
    // 3. Verify GPIO hardware controller is initialized
    if (!gpio_is_ready_dt(&led)) {
        return 0;
    }

    // 4. Configure GPIO pin as output (default inactive state)
    int temp = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
    if (temp < 0) {
        return 0;
    }

    // 5. Main loop: toggle pin state every 500 ms
    while (1) {
        gpio_pin_toggle_dt(&led);
        k_msleep(500);
    }
}
```

---

## 🛠️ How to Build & Run

### Build for Target Board (e.g., STM32)
```bash
west build -b <your_board> firstapp
west flash
```

### Build for QEMU Simulator
```bash
west build -b qemu_cortex_m3 firstapp
west build -t run
```
