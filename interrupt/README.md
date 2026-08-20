# Zephyr RTOS Interrupt & Deferred Workqueue Demo (`interrupt`)

This project demonstrates **Hardware Interrupt Service Routine (ISR) Handling** and **Deferred Execution via the System Workqueue** in Zephyr RTOS, incorporating a software debouncing mechanism for GPIO pushbuttons.

---

## Overview

In real-time embedded systems, Interrupt Service Routines (ISRs) must execute quickly and avoid blocking operations (such as delays, heavy logging, or waiting on locks). Zephyr RTOS provides a powerful pattern for **Deferred Interrupt Processing**:
1. **ISR Phase**: The hardware interrupt triggers `isr_handle()`, which executes minimal logic in interrupt context.
2. **Offloading Phase**: The ISR schedules/reschedules a **delayable work item** (`k_work_reschedule`) to run in thread context on the **System Workqueue**.
3. **Execution Phase**: The system workqueue thread executes `work_handle()`, where pin reading, LED toggling, and logging take place safely.

This pattern also functions as a **Software Debouncing Mechanism** by delaying work execution until contact bounce settles (500 ms).

---

## Directory Structure

```text
interrupt/
├── CMakeLists.txt     # CMake build system environment & executable target
├── prj.conf           # Kconfig features (GPIO driver, Logger, Workqueue stack)
├── app.overlay        # Devicetree overlay defining GPIO LED & Button pins
├── README.md          # Topic documentation & architectural breakdown
└── src/
    └── main.c         # GPIO interrupt setup, ISR, and workqueue handler logic
```

---

## Analysis & Architectural Breakdown

### 1. Devicetree Hardware Mapping (`app.overlay`)
The application defines custom hardware nodes and aliases:
- **LED Node (`ld`)**: Pin `gpio0 2`, configured as `GPIO_ACTIVE_HIGH`.
- **Button Node (`sw`)**: Pin `gpio0 6`, configured with `GPIO_PULL_UP | GPIO_ACTIVE_LOW`.
- **Aliases**: `led` points to `&ld`, `button` points to `&sw`.

### 2. Codebase & Devicetree Observations

| File / Component | Code / Logic | Impact / Explanation |
| :--- | :--- | :--- |
| `src/main.c:16` | `DT_ALIAS(sw0)` | Standard board DTS files define `sw0`. If compiling against `app.overlay` where alias is `button = &sw;`, ensure `sw0` alias is present or update to `DT_ALIAS(button)`. |
| `src/main.c:32-35` | `isr_handle()` | **ISR Context**: Executed directly on hardware interrupt signal. Must remain fast and non-blocking. |
| `src/main.c:34` | `k_work_reschedule(&work_struct, K_MSEC(500))` | Reschedules delayable work. Acts as a **software debouncer** by resetting the 500 ms countdown on subsequent noise/bounces. |
| `src/main.c:18-28` | `work_handle()` | **System Workqueue Thread Context**: Executed safely in thread space after the 500 ms delay expires. |
| `src/main.c:22` | `gpio_pin_get_dt(&button)` | Reads the logical pin state. Since `GPIO_ACTIVE_LOW` is set in Devicetree, pressing the button returns logical `1`. |

---

## Technical Details & Key APIs Used

### 1. Delayable Work Definition
```c
/* Defines a static delayable work item tied to work_handle */
K_WORK_DELAYABLE_DEFINE(work_struct, work_handle);
```

### 2. Interrupt Service Routine (ISR) & Rescheduling
```c
void isr_handle(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    /* Offload work to system workqueue with a 500 ms debouncing delay */
    k_work_reschedule(&work_struct, K_MSEC(500));
}
```

### 3. GPIO Callback & Interrupt Configuration
```c
/* 1. Configure pin for edge-to-active interrupt triggering */
gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);

/* 2. Initialize callback structure with handler and pin mask */
gpio_init_callback(&cb, isr_handle, BIT(button.pin));

/* 3. Register callback with the GPIO port */
gpio_add_callback(button.port, &cb);
```

---

## Kconfig Configuration (`prj.conf`)

- `CONFIG_GPIO=y`: Enables the Zephyr GPIO driver subsystem.
- `CONFIG_LOG=y`: Enables the Zephyr Logging subsystem.
- `CONFIG_LOG_MODE_IMMEDIATE=n`: Defers log output to avoid logging latency in sensitive contexts.
- `CONFIG_SYSTEM_WORKQUEUE_STACK_SIZE=1024`: Allocates a 1 KB stack to the system workqueue thread.
- `CONFIG_HW_STACK_PROTECTION=y`: Enables hardware stack protection for thread safety.

---

## How to Build and Run

From the root of your Zephyr RTOS workspace:

### 1. Build for QEMU Simulator
```bash
west build -b qemu_cortex_m3 interrupt -p always
```

### 2. Run in Simulator
```bash
west build -t run
```

### 3. Build & Flash to Physical Target Board (e.g., STM32 / NXP / Nordic)
```bash
west build -b <your_board_name> interrupt
west flash
```

---

## Expected Output

When the control button is pressed, the system logs the deferred handler entry and toggles the LED state:

```text
*** Booting Zephyr OS build v3.5.0 ***
[00:00:01.500,000] <inf> app: ENTERED THE WORKING HANDLE
[00:00:01.500,000] <inf> app: Button Pressed
```
