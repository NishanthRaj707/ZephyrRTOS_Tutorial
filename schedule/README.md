# Module 4: `schedule` - Kernel Scheduler Control & Timeslicing

The **`schedule`** module explores advanced **Zephyr RTOS Scheduler** mechanisms, including **Scheduler Locking (`k_sched_lock` / `k_sched_unlock`)**, **Preemptive Round-Robin Timeslicing**, and prioritizing equal-priority threads.

---

## 🔑 Key Concepts Covered

1. **Scheduler Locking (`k_sched_lock()` & `k_sched_unlock()`)**:
   - `k_sched_lock()` prevents the scheduler from performing preemption. The calling thread remains CPU owner until it unlocks or performs a blocking operation (like `k_msleep()`).
   - Useful for short, non-blocking critical sections where preemption by other threads must be suppressed without disabling hardware interrupts.
2. **Round-Robin Timeslicing (`CONFIG_TIMESLICING=y`)**:
   - `CONFIG_TIMESLICING=y`: Enables time-slicing for preemptive threads of equal priority.
   - `CONFIG_TIMESLICE_SIZE=10`: Each thread gets a maximum slice of 10 ms before being preempted by an equal-priority thread.
   - `CONFIG_TIMESLICE_PRIORITY=0`: Sets the priority threshold above which timeslicing applies.
3. **Priority Setup (`prj.conf`)**:
   - `CONFIG_NUM_COOP_PRIORITIES=3`
   - `CONFIG_NUM_PREEMPT_PRIORITIES=5`
4. **Equal Priority Multithreading**:
   - Defining multiple threads with identical priorities (`PRIORITY 3`) to observe scheduler context switches.

---

## 📁 File Structure

- [CMakeLists.txt](file:///home/bava/Desktop/ZephyrRTOS_Tutorial/schedule/CMakeLists.txt): Project compilation rules.
- [prj.conf](file:///home/bava/Desktop/ZephyrRTOS_Tutorial/schedule/prj.conf): Configures Timeslicing parameters, priorities, and logging.
- [app.overlay](file:///home/bava/Desktop/ZephyrRTOS_Tutorial/schedule/app.overlay): Hardware Devicetree overlay.
- [src/main.c](file:///home/bava/Desktop/ZephyrRTOS_Tutorial/schedule/src/main.c): Application source code implementing locked sections and dual threads.

---

## 📝 Code Breakdown

```c
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>

#define STACK 1024
#define PRIORITY 3

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

// Thread A: Periodic Logging
void thread_a(void* p1, void* p2, void* p3)
{
    while (1) {
        LOG_INF("I AM THREAD A ----------------------------------");
        k_msleep(1000);
    }
}

// Thread B: Demonstrates Scheduler Locking
void thread_b(void* p1, void* p2, void* p3)
{
    k_sched_lock();  // Lock scheduler: prevent preemption
    LOG_INF("i am THREAD b --------------------------------------------");
    gpio_pin_toggle_dt(&led);
    k_msleep(2000);  // Note: blocking sleeps unlock scheduler temporarily
    LOG_INF("I AM EXIT");
    k_sched_unlock(); // Unlock scheduler
}

// Both threads created at equal priority (PRIORITY 3)
K_THREAD_DEFINE(thread1, STACK, thread_a, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(thread2, STACK, thread_b, NULL, NULL, NULL, PRIORITY, 0, 0);
```

---

## ⚠️ Important Scheduler Locking Rules

> [!IMPORTANT]
> Holding `k_sched_lock()` only prevents **thread preemption**. It does **NOT** disable hardware interrupts (ISRs).
> Furthermore, if a thread calls a blocking API (such as `k_msleep()` or `k_sem_take()`) while holding the scheduler lock, the kernel will yield CPU execution to another thread until the sleep duration/blocking condition completes!

---

## 🛠️ How to Build & Run

```bash
west build -b qemu_cortex_m3 schedule
west build -t run
```
