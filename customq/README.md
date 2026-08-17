# Zephyr RTOS Custom Workqueue Demo (`customq`)

This project demonstrates how to initialize, start, and submit work items to a user-defined **Custom Workqueue (`struct k_work_q`)** in Zephyr RTOS, instead of using the shared system workqueue.

---

## Overview

While Zephyr provides a default system workqueue (`k_work`), dedicated application modules often require custom workqueues to:
- Avoid blocking or starving system-wide work items.
- Control thread stack size and execution priority independently.
- Isolate task processing for specific peripheral subsystems.

This module demonstrates creating a custom workqueue, encapsulating custom metadata inside container structs using `CONTAINER_OF()`, and submitting work items dynamically using `k_work_submit_to_queue()`.

---

## Directory Structure

```text
customq/
├── CMakeLists.txt     # CMake build configuration
├── prj.conf           # Kernel Kconfig configuration (GPIO, Logging)
├── app.overlay        # Devicetree overlay mapping LED pin
├── README.md          # Topic documentation & analysis
└── src/
    └── main.c         # Custom workqueue initialization and application logic
```

---

## Analysis & Resolved Errors in Codebase

During code analysis of `customq`, several compilation errors and anti-patterns were identified and fixed:

| File | Error / Anti-Pattern Identified | Corrective Action Applied |
| :--- | :--- | :--- |
| `src/main.c` | Python function keyword `def` used instead of `void` for `event_handler` | Changed to `void event_handler(struct k_work *work)` |
| `src/main.c` | Pointer `struct k_work* work_q` in `led_state` container struct | Changed to embedded object `struct k_work work;` for `CONTAINER_OF` compatibility |
| `src/main.c` | Syntax error `struct *state = CONTAINER_OF(...)` | Fixed pointer declaration to `led_state *state = CONTAINER_OF(...)` |
| `src/main.c` | Semicolons `;` inside struct initializers `.status = 1;` | Replaced semicolons with commas `,` in struct initializers |
| `src/main.c` | Missing trailing semicolon `;` after `led_off` struct definition | Added `;` after closing brace |
| `src/main.c` | Macro undefined `STACK_SIZE` (code defined `STACK`) | Standardized on `#define STACK_SIZE 2048` and `K_THREAD_STACK_SIZEOF(stack_q)` |
| `src/main.c` | Missing semicolons on `LOG_INF` and `int ret` | Added semicolons `;` |
| `src/main.c` | Member mismatch `&led_on.work` vs `work_q` in struct | Standardized struct member name to `work` |
| `src/main.c` | Use of internal private macro `Z_WORK_INITIALIZER` | Replaced with public API macro `K_WORK_INITIALIZER(event_handler)` |
| `app.overlay` | Invalid Devicetree flag `ACTIVE_HIGH` | Replaced with standard macro `GPIO_ACTIVE_HIGH` |
| `prj.conf` | Deprecated option `CONFIG_LOG_IMMEDIATE=n` | Updated to `CONFIG_LOG_MODE_IMMEDIATE=n` |

---

## Technical Details & APIs Used

### 1. Custom Stack & Workqueue Definition
```c
K_THREAD_STACK_DEFINE(stack_q, STACK_SIZE);
static struct k_work_q custom_work_q;
```

### 2. Workqueue Initialization & Startup
```c
k_work_queue_init(&custom_work_q);
k_work_queue_start(&custom_work_q, stack_q, K_THREAD_STACK_SIZEOF(stack_q), K_PRIO_COOP(1), NULL);
```

### 3. Submitting Work Items
```c
k_work_submit_to_queue(&custom_work_q, &led_on.work);
```

### 4. Container Retrieval via `CONTAINER_OF()`
```c
led_state *state = CONTAINER_OF(work, led_state, work);
```
Allows passing custom data (`status`) alongside work structures into handler callbacks.

---

## How to Build and Run

From the root of your Zephyr workspace:

```bash
# Build for target board
west build -b <your_board_name> customq

# Flash to connected board
west flash
```

---

## Expected Output

Serial monitor log output:
```text
[00:00:00.000,000] <inf> app: BAVA is starting...
[00:00:00.000,000] <inf> app: LED is ON
[00:00:01.000,000] <inf> app: LED is OFF
[00:00:02.000,000] <inf> app: LED is ON
[00:00:03.000,000] <inf> app: LED is OFF
```
