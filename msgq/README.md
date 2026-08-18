# Zephyr RTOS Message Queue (`msgq`) Tutorial

This directory contains a **Zephyr RTOS** application demonstrating inter-thread communication using **Message Queues (`k_msgq`)**.

---

## 1. Overview

A Message Queue (`k_msgq`) is a kernel object that allows threads and ISRs to send and receive fixed-size data messages asynchronously in a FIFO (First-In, First-Out) order.

- **Producer (`thread_send`)**: Generates and sends messages to the message queue using `k_msgq_put()`.
- **Consumer (`thread_get`)**: Retrieves and processes messages from the message queue using `k_msgq_get()`.

---

## 2. Identified Errors & Issues in Initial Code

During static analysis of the `msgq` project, the following **7 errors and issues** were identified in `src/main.c` and `prj.conf`:

### 🚨 Critical Errors (Compilation & Runtime Crashes)

1. **Compilation Error: Incomplete/Undefined Type in `K_MSGQ_DEFINE`**
   - **Line 7**: `K_MSGQ_DEFINE(my_msgq, sizeof(struct msg), 5, 4);`
   - **Issue**: `struct msg` is referenced in `sizeof()` but is never defined anywhere in the codebase. This causes a compilation error (`incomplete type 'struct msg'`).

2. **Fatal HardFault / MPU Fault (Uninitialized Pointer Dereference & Data Mismatch)**
   - **Lines 16–20 & 26–32**:
     - `send_handler()` sends integer values: `int i; k_msgq_put(&my_msgq, &i, ...)` (4-byte payload).
     - `get_handler()` declares a pointer `structure* msg;` and passes `&msg` (`structure**`) to `k_msgq_get(&my_msgq, &msg, ...)`.
   - **Issue**: `k_msgq_get` overwrites the raw memory of the pointer variable `msg` with the integer value `i` (e.g. `0`, `1`, `2`). Line `LOG_INF("Received: %d", msg->count)` then dereferences address `0x00000000`, resulting in a **NULL Pointer Dereference / HardFault crash**.

3. **Logic Flaw: Consumer Thread Terminates After Single Execution**
   - **Lines 14–22**: `get_handler()` receives one message and immediately returns without a `while (1)` loop.
   - **Issue**: Returning from a thread entry function terminates the thread in Zephyr. Subsequent messages sent by `send_handler()` will quickly fill the queue and block indefinitely.

4. **Thread Handler Function Signature Mismatch**
   - **Lines 14 & 24**: `void get_handler()` and `void send_handler()` do not accept arguments.
   - **Issue**: Zephyr thread entry handlers created with `K_THREAD_DEFINE` must match `void entry(void *p1, void *p2, void *p3)`. Missing parameter prototypes cause compiler warnings or stack frame mismatches on strict ABI architectures.

---

### ⚠️ Functional & Style Warnings

5. **Producer Queue Stalling / Overflows**
   - **Lines 26–32**: `send_handler()` attempts to send 1000 items in a rapid loop without delays. Since the queue depth is only 5 items and the consumer thread exits after 1 read, `thread_send` stalls waiting on full queue timeouts (`K_MSEC(1000)`).

6. **Unused / Confusing `struct k_work` inside Message Payload**
   - **Lines 9–12**: `typedef struct { struct k_work work; uint8_t count; } structure;`
   - `k_work` is for Zephyr Workqueues, not Message Queues. `k_msgq` copies data by value and does not use `k_work`.

7. **Deprecated Kconfig Symbol in `prj.conf`**
   - **Line 2**: `CONFIG_LOG_IMMEDIATE=n` is deprecated in recent Zephyr releases in favor of default deferred logging or `CONFIG_LOG_MODE_IMMEDIATE=y`.

---

## 3. Corrected Implementation (`src/main.c`)

Below is the fixed version of `src/main.c`:

```c
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

/* 1. Define message data structure */
typedef struct {
    uint32_t id;
    uint8_t count;
} msg_data_t;

/* 2. Define message queue: capacity of 5 messages, aligned to 4 bytes */
K_MSGQ_DEFINE(my_msgq, sizeof(msg_data_t), 5, 4);

/* 3. Consumer Thread Handler */
void get_handler(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    msg_data_t rx_msg;

    while (1) {
        /* Wait indefinitely for incoming messages */
        if (k_msgq_get(&my_msgq, &rx_msg, K_FOREVER) == 0) {
            LOG_INF("Received Msg ID: %d, Count: %d", rx_msg.id, rx_msg.count);
        }
    }
}

/* 4. Producer Thread Handler */
void send_handler(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    msg_data_t tx_msg;

    for (uint32_t i = 0; i < 1000; i++) {
        tx_msg.id = i;
        tx_msg.count = (uint8_t)(i % 256);

        if (k_msgq_put(&my_msgq, &tx_msg, K_MSEC(1000)) == 0) {
            LOG_INF("Sent Msg ID: %d", tx_msg.id);
        } else {
            LOG_WRN("Failed to send Msg ID: %d (Queue Full)", tx_msg.id);
        }

        /* Pacing sleep between sends */
        k_msleep(500);
    }
}

/* 5. Define Threads */
K_THREAD_DEFINE(thread_get, 2048, get_handler, NULL, NULL, NULL, 2, 0, 0);
K_THREAD_DEFINE(thread_send, 2048, send_handler, NULL, NULL, NULL, 1, 0, 0);

int main(void)
{
    LOG_INF("Message Queue Demo Started");
    return 0;
}
```

---

## 4. Building and Running

### Build Command (Example for `qemu_cortex_m3`)
```bash
west build -b qemu_cortex_m3 msgq
```

### Run on QEMU
```bash
west build -t run
```
