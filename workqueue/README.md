# Zephyr RTOS System Workqueue Demo

This project demonstrates the usage of the **Zephyr RTOS System Workqueue (`k_work`) API** to offload tasks asynchronously to the system workqueue thread.

## Overview

In Zephyr RTOS, workqueues provide a mechanism to defer work from an interrupt service routine (ISR) or a cooperative/preemptive thread to a dedicated kernel workqueue thread. This project demonstrates:
- Static definition of work items using `K_WORK_DEFINE`.
- Submitting work items to the default System Workqueue using `k_work_submit()`.
- Controlling hardware (GPIO / LED) asynchronously inside work item handler callbacks.
- Configuring System Workqueue priority and stack size using Kconfig settings (`prj.conf`).
- Hardware mapping using DeviceTree overlay (`app.overlay`).

---

## Directory Structure

```text
workqueue/
├── CMakeLists.txt     # Build system configuration
├── prj.conf           # Kernel and feature configurations (Workqueue, GPIO, Logging)
├── app.overlay        # Board-specific DeviceTree overlay (LED mapping)
├── README.md          # Project documentation
└── src/
    └── main.c         # Application entry point and workqueue logic
```

---

## Key Files & Technical Explanation

### 1. `app.overlay`
Extends the base board DeviceTree configuration by adding a custom LED node (`led_0`) attached to GPIO 0, Pin 2 with active-high polarity, mapped to the alias `led`:
- **Alias:** `led` -> `&status_led`
- **GPIO Pin:** `gpio0 2 GPIO_ACTIVE_HIGH`

### 2. `prj.conf`
Configures Zephyr kernel parameters for this application:
- `CONFIG_GPIO=y`: Enables GPIO driver support.
- `CONFIG_LOG=y`: Enables Zephyr logging module.
- `CONFIG_SYSTEM_WORKQUEUE_PRIORITY=-1`: Sets system workqueue thread priority (cooperative priority `-1`).
- `CONFIG_SYSTEM_WORKQUEUE_STACK_SIZE=2048`: Configures 2048 bytes of stack memory for the system workqueue.

### 3. `src/main.c`
- **Work Handler Functions:**
  - `led_on_handler()`: Sets the LED GPIO pin state to HIGH (`1`) and logs `"LED ON"`.
  - `led_off_handler()`: Sets the LED GPIO pin state to LOW (`0`) and logs `"LED OFF"`.
- **Work Item Definitions:**
  - `K_WORK_DEFINE(led_on, led_on_handler);`
  - `K_WORK_DEFINE(led_off, led_off_handler);`
- **Main Loop:**
  - Verifies GPIO controller readiness (`gpio_is_ready_dt`).
  - Configures the GPIO pin as an inactive output (`gpio_pin_configure_dt`).
  - Periodically submits `led_on` and `led_off` work items to the system workqueue with `k_work_submit()` separated by `k_msleep(1000)`.

### 4. `CMakeLists.txt`
Declares minimum CMake version, includes standard Zephyr package boilerplate, names the project `zephyr_workqueue_demo`, and adds `src/main.c` to target sources.

---

## How it Works (Execution Flow)

1. Upon booting, `main()` initializes the LED GPIO pin.
2. The main thread enters a `while(1)` loop.
3. Every 1000 ms, `main()` calls `k_work_submit(&led_on)` or `k_work_submit(&led_off)`.
4. The kernel places the submitted work structure into the system workqueue queue.
5. The kernel's system workqueue thread processes the queue, invoking `led_on_handler()` or `led_off_handler()`, which toggles the physical LED state and outputs log messages.

---

## How to Build and Flash

From the root of your Zephyr workspace:

```bash
# Build for your target board (e.g., esp32_devkitc_wroom or nrf52840dk_nrf52840)
west build -b <your_board_name> workqueue

# Flash to connected hardware target
west flash
```

---

## Expected Output

Serial terminal log output:
```text
[00:00:00.000,000] <inf> app: BAVA IS STARTING ..........
[00:00:00.000,000] <inf> app: LED ON
[00:00:01.000,000] <inf> app: LED OFF
[00:00:02.000,000] <inf> app: LED ON
[00:00:03.000,000] <inf> app: LED OFF
```
