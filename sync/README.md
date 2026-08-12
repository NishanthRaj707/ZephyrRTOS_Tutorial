# Module 5: `sync` - Inter-Thread Synchronization & IPC Primitives

The **`sync` (Synchronization)** module covers core **Inter-Thread Communication (IPC)** and **Synchronization Primitives** provided by the Zephyr RTOS kernel.

---

## 🔑 Key Synchronization Primitives in Zephyr

| Primitive | Kernel Structure | Primary Use Case | Key Features |
| :--- | :--- | :--- | :--- |
| **Semaphore** | `struct k_sem` | Signal notification & resource counting | Can be signaled from ISR or Thread; non-recursive count tracking |
| **Mutex** | `struct k_mutex` | Mutual exclusion for shared resources | Thread-only ownership; priority inheritance to prevent priority inversion |
| **Event Flags** | `struct k_event` | Multi-condition bitmask matching | Wait on ALL or ANY set flag bits simultaneously |
| **Message Queue** | `struct k_msgq` | Ring-buffer inter-thread data transfer | Thread-safe, fixed-size element message passing |

---

## 📘 Detailed Breakdown of Primitives

### 1. Semaphores (`k_sem`)
Semaphores track resource availability or signal events between threads and ISRs.

```c
#include <zephyr/kernel.h>

// Define static semaphore with initial count 0, max limit 1 (Binary Semaphore)
K_SEM_DEFINE(my_sem, 0, 1);

void producer_thread(void *p1, void *p2, void *p3)
{
    // Give/Signal the semaphore (can be called from ISR)
    k_sem_give(&my_sem);
}

void consumer_thread(void *p1, void *p2, void *p3)
{
    // Wait/Take the semaphore (blocks thread until given or timeout)
    if (k_sem_take(&my_sem, K_FOREVER) == 0) {
        // Resource available / event received
    }
}
```

---

### 2. Mutexes (`k_mutex`)
Mutexes protect shared data structures or hardware peripherals against concurrent access.

> [!IMPORTANT]
> **Priority Inheritance**: If a low-priority thread holds a mutex and a high-priority thread attempts to acquire it, the kernel temporarily boosts the low-priority thread's priority to match the high-priority thread. This prevents **Priority Inversion**.

```c
#include <zephyr/kernel.h>

// Statically define a mutex
K_MUTEX_DEFINE(my_mutex);

void worker_thread(void *p1, void *p2, void *p3)
{
    // Lock critical section
    if (k_mutex_lock(&my_mutex, K_MSEC(100)) == 0) {
        // Access shared hardware peripheral or buffer safely
        
        // Unlock critical section
        k_mutex_unlock(&my_mutex);
    }
}
```

---

### 3. Event Flags (`k_event`)
Events allow threads to wait on a bitmask of conditions.

```c
#include <zephyr/kernel.h>

K_EVENT_DEFINE(my_event);

#define EVENT_FLAG_A BIT(0)
#define EVENT_FLAG_B BIT(1)

// Sender
void send_event(void) {
    k_event_post(&my_event, EVENT_FLAG_A);
}

// Receiver
void wait_event(void) {
    uint32_t events = k_event_wait(&my_event, EVENT_FLAG_A | EVENT_FLAG_B, false, K_FOREVER);
}
```

---

### 4. Message Queues (`k_msgq`)
Message queues provide thread-safe FIFO data buffer passing.

```c
#include <zephyr/kernel.h>

struct data_item {
    uint32_t id;
    uint32_t val;
};

// Queue holding up to 10 items of size struct data_item
K_MSGQ_DEFINE(my_msgq, sizeof(struct data_item), 10, 4);

void send_data(struct data_item *item) {
    k_msgq_put(&my_msgq, item, K_NO_WAIT);
}

void receive_data(struct data_item *item) {
    k_msgq_get(&my_msgq, item, K_FOREVER);
}
```

---

## 💻 Practice Lab Exercise

Use this folder to implement a Producer-Consumer application:
1. Open [src/main.c](file:///home/bava/Desktop/ZephyrRTOS_Tutorial/sync/src/main.c).
2. Define a Semaphore using `K_SEM_DEFINE(sync_sem, 0, 1)`.
3. Create a **Producer Thread** that calls `k_sem_give(&sync_sem)` every 1000 ms.
4. Create a **Consumer Thread** that calls `k_sem_take(&sync_sem, K_FOREVER)` and toggles the LED upon receiving the signal.

---

## 🛠️ How to Build & Run

```bash
west build -b qemu_cortex_m3 sync
west build -t run
```
