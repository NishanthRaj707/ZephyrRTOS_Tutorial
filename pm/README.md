# Zephyr RTOS Power Management & ESP32 Deep Sleep Demo (`pm`)

This project demonstrates **Power Management (PM)** and **Deep Sleep / Poweroff Modes** in Zephyr RTOS on Espressif targets (ESP32) using `<zephyr/sys/poweroff.h>`, `<esp_sleep.h>`, RTC memory persistence (`RTC_DATA_ATTR`), and wake-up event triggers (Timer and EXT0 GPIO Pin).

---

## Overview

Power Management is critical for battery-powered embedded systems. Zephyr RTOS provides power management frameworks (`CONFIG_PM`, `sys_poweroff()`) integrated with SoC vendor deep sleep APIs (`esp_sleep_enable_timer_wakeup`, `esp_sleep_enable_ext0_wakeup`). This demo showcases:
- **RTC Fast Memory State Persistence**: Maintaining state (`wakeup_counter`) across deep sleep resets using the `RTC_DATA_ATTR` attribute, preserving data while main CPU power is off.
- **Wakeup Cause Diagnostics**: Identifying why the system woke up using `esp_sleep_get_wakeup_cause()`.
- **Configuring Wakeup Triggers**: Enabling 5-second timer wakeups and GPIO pin interrupts (`GPIO_NUM_0`).
- **Poweroff / Deep Sleep Entry**: Transitioning the system into ultra-low power state via `sys_poweroff()`.

---

## Directory Structure

```text
pm/
├── CMakeLists.txt     # CMake build system rules
├── prj.conf           # Kconfig options (PM, System Poweroff, Tickless Kernel)
├── app.overlay        # Devicetree overlay (Default/empty)
├── README.md          # Topic documentation & design analysis
└── src/
    └── main.c         # RTC counter, wake cause query, wakeup setup & sys_poweroff
```

---

## Architecture & System Flow

```mermaid
sequenceDiagram
    autonumber
    participant App as Main Application
    participant PM as Zephyr System Poweroff
    participant HAL as ESP32 Sleep Subsystem
    participant RTC as RTC Fast Memory

    App->>RTC: Increment wakeup_counter (Persisted in RTC RAM)
    App->>HAL: esp_sleep_get_wakeup_cause()
    HAL-->>App: Return Cause (Timer / EXT0 GPIO / Cold Boot)
    App->>App: Log Boot Count & Wakeup Source
    App->>HAL: esp_sleep_enable_timer_wakeup(5,000,000 us)
    App->>HAL: esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0)
    App->>PM: sys_poweroff()
    PM->>HAL: Enter ESP32 Deep Sleep State
    Note over HAL,RTC: CPU powered off; RTC Memory remains powered
```

---

## Key Technical Features & APIs Used

### 1. Persistent RTC Memory (`RTC_DATA_ATTR`)
Variables declared with `RTC_DATA_ATTR` are assigned to ESP32 RTC fast memory, allowing data retention across deep sleep reset cycles:
```c
static RTC_DATA_ATTR uint32_t wakeup_counter = 0;
```

### 2. Querying Wakeup Cause (`esp_sleep_get_wakeup_cause`)
Evaluates the hardware event that triggered bootup:
```c
esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();

switch (cause) {
case ESP_SLEEP_WAKEUP_TIMER:
    LOG_INF("wakeup cause timer");
    break;
case ESP_SLEEP_WAKEUP_EXT0:
    LOG_INF("wakeup cause external pin");
    break;
default:
    LOG_INF("wakeup cause unknown");
    break;
}
```

### 3. Configuring Wakeup Triggers & Poweroff
- **Timer Wakeup**: `esp_sleep_enable_timer_wakeup(5000000);` (5 seconds = 5,000,000 microseconds).
- **EXT0 GPIO Wakeup**: `esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0);` (Triggers on GPIO0 Low logic level).
- **System Poweroff**: `sys_poweroff();` places the system into deep sleep state.

---

## Code Analysis & Bug Fixes Applied

| File / Line | Issue Description | Impact | Corrective Fix |
| :--- | :--- | :--- | :--- |
| `src/main.c:7` | Typo: `LOG_MOUDLE_REGISTER(app,LOG_LEVEL_INF);` | Compiler syntax error (`LOG_MOUDLE_REGISTER` undeclared). | Corrected macro name to `LOG_MODULE_REGISTER`. |
| `src/main.c:6` | Missing Header: `GPIO_NUM_0` undeclared | Compiler error (`GPIO_NUM_0` undeclared). | Added `#include <driver/gpio.h>` header. |

---

## Kconfig Configuration (`prj.conf`)

- `CONFIG_PM=y`: Enables Zephyr Power Management framework.
- `CONFIG_PM_DEVICE=y`: Enables device power management subsystem.
- `CONFIG_POWEROFF=y`: Enables system poweroff (`sys_poweroff`) API.
- `CONFIG_TICKLESS_KERNEL=y`: Enables tickless idle for power efficiency.
- `CONFIG_LOG=y`, `CONFIG_LOG_DEFAULT_LEVEL=3`: Serial logger configuration.

---

## How to Build and Flash

From the root of your Zephyr RTOS workspace:

### 1. Build for ESP32 Target Board
```bash
west build -p always -b esp32_devkitc_wroom pm
```

### 2. Flash to ESP32 Hardware
```bash
west flash
```

---

## Expected Output

Serial log output upon initial boot and subsequent 5-second timer wakeups:

```text
*** Booting Zephyr OS build v3.7.0 ***
[00:00:00.000,000] <inf> app: woken up 1 times
[00:00:00.000,000] <inf> app: wakeup cause 0
[00:00:00.000,000] <inf> app: wakeup cause unknown
--- System enters Poweroff / Deep Sleep ---

*** Booting Zephyr OS build v3.7.0 ***
[00:00:00.000,000] <inf> app: woken up 2 times
[00:00:00.000,000] <inf> app: wakeup cause 4
[00:00:00.000,000] <inf> app: wakeup cause timer
```
