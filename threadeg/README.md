# Module 3: `threadeg` - Multithreading & Thread Priorities

The **`threadeg` (Thread Example)** module introduces **multithreading** in Zephyr RTOS, illustrating the core differences between **Cooperative Threads** and **Preemptive Threads**, thread priority scheduling, static thread creation, and the Zephyr Logging Subsystem.

---

## 🔑 Key Concepts Covered

1. **Cooperative Threads vs. Preemptive Threads**:
   - **Cooperative Threads (Negative Priority, e.g., `-2`)**:
     - Cooperative thread priorities range from `-K_PRIO_COOP(N)` to `-1`.
     - Once running, a cooperative thread **cannot be preempted** by another thread of equal or lower priority (or even higher-priority preemptive threads).
     - It continues executing until it explicitly yields or blocks (e.g., calling `k_msleep()`, `k_yield()`, or waiting on a kernel object).
   - **Preemptive Threads (Positive Priority, e.g., `1`)**:
     - Preemptive thread priorities range from `0` to `K_PRIO_PREEMPT(N)`.
     - A preemptive thread can be interrupted at any moment by a higher-priority preemptive thread or an ISR.
2. **Static Thread Creation (`K_THREAD_DEFINE`)**:
   - Macro syntax: `K_THREAD_DEFINE(name, stack_size, entry_func, p1, p2, p3, priority, options, delay)`.
   - Allocates stack memory and initializes thread control blocks at compile time.
3. **Zephyr Logging Subsystem (`CONFIG_LOG=y`)**:
   - Registered using `LOG_MODULE_REGISTER(app, LOG_LEVEL_INF)`.
   - Asynchronous log processing (`CONFIG_LOG_MODE_IMMEDIATE=n`).
   - Logging macros: `LOG_INF()`, `LOG_ERR()`.

---

## 📁 File Structure

- [CMakeLists.txt](file:///home/bava/Desktop/ZephyrRTOS_Tutorial/threadeg/CMakeLists.txt): Build targets.
- [prj.conf](file:///home/bava/Desktop/ZephyrRTOS_Tutorial/threadeg/prj.conf): Configures `CONFIG_NUM_COOP_PRIORITIES=5`, `CONFIG_NUM_PREEMPT_PRIORITIES=10`, `CONFIG_LOG=y`, `CONFIG_THREAD_NAME=y`.
- [app.overlay](file:///home/bava/Desktop/ZephyrRTOS_Tutorial/threadeg/app.overlay): Devicetree overlay for GPIO LED.
- [src/main.c](file:///home/bava/Desktop/ZephyrRTOS_Tutorial/threadeg/src/main.c): Application with cooperative and preemptive threads.


---

## 📊 Execution Behavior Analysis

1. **Startup**: Kernel initializes statically defined threads (`coop` and `preempt`).
2. **Cooperative Execution**: Because `coop` has priority `-2` (cooperative), it gets selected first. It logs `"COOPERATIVE THREAD IS RUNNING."`.
3. **Yielding via Sleep**: `coop` invokes `k_msleep(3000)`. Putting itself into the sleeping state allows the scheduler to pick `preempt` (Priority `1`).
4. **Preemptive Execution**: `preempt` logs `"PREEMPTIVE THREAD IS RUNNING."` and starts toggling the LED every 500 ms.
5. **Wakeup**: After 3 seconds, `coop` wakes up. Since `-2` is higher priority than `1`, `coop` immediately resumes execution.

---

## 🛠️ How to Build & Run

```bash
west build -b qemu_cortex_m3 threadeg
west build -t run
```
