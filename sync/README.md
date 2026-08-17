# Module 5: `sync` - Inter-Thread Synchronization & IPC Primitives

The **`sync` (Synchronization)** module covers core **Inter-Thread Communication (IPC)** and **Synchronization Primitives** provided by the Zephyr RTOS kernel.

---

## 🔑 Key Synchronization Primitives in Zephyr

| Primitive | Kernel Structure | Primary Use Case | Key Features |
| :--- | :--- | :--- | :--- |
| **Semaphore** | `struct k_sem` | Signal notification & resource counting | Can be signaled from ISR or Thread; non-recursive count tracking |
| **Mutex** | `struct k_mutex` | Mutual exclusion for shared resources | Thread-only ownership; priority inheritance to prevent priority inversion |
---

### Mutexes (`k_mutex`)
Mutexes protect shared data structures or hardware peripherals against concurrent access.

> [!IMPORTANT]
> **Priority Inheritance**: If a low-priority thread holds a mutex and a high-priority thread attempts to acquire it, the kernel temporarily boosts the low-priority thread's priority to match the high-priority thread. This prevents **Priority Inversion**.

---

## 💻 Practice Lab Exercise

Use this folder to implement a Producer-Consumer application:
1. Open [src/main.c].
2. Define a Semaphore using `K_SEM_DEFINE(sync_sem, 0, 1)`.
3. Create a **Producer Thread** that calls `k_sem_give(&sync_sem)` every 1000 ms.
4. Create a **Consumer Thread** that calls `k_sem_take(&sync_sem, K_FOREVER)` and toggles the LED upon receiving the signal.

---

## 🛠️ How to Build & Run

```bash
west build -b qemu_cortex_m3 sync
west build -t run
```
