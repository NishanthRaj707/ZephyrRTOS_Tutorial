# Module 2: `hwins` - Hardware & System Environment Inspector

The **`hwins` (Hardware Inspector)** module demonstrates how to inspect system hardware configurations at compile-time and runtime using **Devicetree macros** and **Kconfig environment variables** in Zephyr RTOS, while outputting diagnostic logs over the UART serial console using `printk()`.

---

## 🔑 Key Concepts Covered

1. **Kconfig System Inspection**:
   - Querying target CPU architecture using preprocessor flags (`CONFIG_ARM`, `CONFIG_XTENSA`, `CONFIG_RISCV`).
   - Checking feature status using `IS_ENABLED(CONFIG_CONSOLE)` and `IS_ENABLED(CONFIG_GPIO)`.
   - Accessing kernel system clock settings (`CONFIG_SYS_CLOCK_TICKS_PER_SEC`).
2. **Devicetree Macro Metadata Extraction**:
   - `DT_PROP(LED, label)`: Reads node property values (e.g., `"internal-led"`) at compile time.
   - `DT_GPIO_PIN(LED, gpios)`: Extracts physical pin numbers directly from Devicetree definitions.
   - `DT_GPIO_FLAGS(LED, gpios)`: Inspects active flags (e.g., `GPIO_ACTIVE_HIGH`).
   - `led.port->name`: Queries the driver device name at runtime (e.g., `"GPIO_1"` / `"gpiob"`).
3. **Serial Console & UART Subsystem (`prj.conf`)**:
   - `CONFIG_SERIAL=y`, `CONFIG_CONSOLE=y`, `CONFIG_PRINTK=y`: Enables string formatting and UART transmission.
   - `CONFIG_SYS_CLOCK_TICKS_PER_SEC=1000`: Sets 1 ms system tick resolution.

---

## 📁 File Structure

- [CMakeLists.txt](file:///home/bava/Desktop/ZephyrRTOS_Tutorial/hwins/CMakeLists.txt): Build configuration.
- [prj.conf](file:///home/bava/Desktop/ZephyrRTOS_Tutorial/hwins/prj.conf): Configures Serial UART, console, printk, tick rate, and GPIO.
- [app.overlay](file:///home/bava/Desktop/ZephyrRTOS_Tutorial/hwins/app.overlay): Devicetree mapping for LED.
- [src/main.c](file:///home/bava/Desktop/ZephyrRTOS_Tutorial/hwins/src/main.c): Diagnostic hardware inspector application logic.

---

## 🖥️ Expected Serial Output

When executed on a target board or simulator, `hwins` prints a diagnostic report to the UART terminal:

```text
===================================================
       ZEPHYR RTOS: HARDWARE INSPECTOR & BLINK     
===================================================

[KCONFIG] Architecture Target:
          -> ARM Cortex-M Series (e.g., STM32)
[KCONFIG] Console Subsystem Status : ENABLED (1)
[KCONFIG] GPIO Driver Subsystem    : ENABLED (1)
[KCONFIG] System Tick Rate         : 1000 Hz

[DT] LED Node Label                 : internal-led
[DT] GPIO Device                    : gpiob@40020400
[DT] GPIO Pin Number               : 2
[DT] GPIO Active Flag              : 0
LED device is ready
```

---

## 📝 Code Breakdown

### Section A: Kconfig Querying
```c
printk("[KCONFIG] Architecture Target:\n");
#if defined(CONFIG_ARM)
    printk("          -> ARM Cortex-M Series (e.g., STM32)\n");
#elif defined(CONFIG_XTENSA)
    printk("          -> Xtensa Architecture (e.g., ESP32)\n");
#endif

printk("[KCONFIG] Console Subsystem Status : ENABLED (%d)\n", IS_ENABLED(CONFIG_CONSOLE));
printk("[KCONFIG] System Tick Rate         : %d Hz\n", CONFIG_SYS_CLOCK_TICKS_PER_SEC);
```

### Section B: Devicetree Querying
```c
#define LABEL DT_PROP(LED, label)
#define PIN   DT_GPIO_PIN(LED, gpios)
#define FLAG  DT_GPIO_FLAGS(LED, gpios)

printk("[DT] LED Node Label   : %s\n", LABEL);
printk("[DT] GPIO Device      : %s\n", led.port->name);
printk("[DT] GPIO Pin Number  : %d\n", PIN);
```

---

## 🛠️ How to Build & Run

```bash
# Build and flash to board
west build -b <your_board> hwins
west flash

# View serial console logs (using picocom/minicom/screen)
picocom -b 115200 /dev/ttyACM0
```
