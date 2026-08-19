# Zephyr RTOS FIFO Queue Demo (`fifo`)

This project demonstrates how to implement a **Producer-Consumer Multithreading Model** using Zephyr RTOS's **Kernel FIFO Queue (`k_fifo`) API** for inter-thread communication (IPC).

---

## Overview

In Zephyr RTOS, a **FIFO (First-In, First-Out queue)** is a kernel service that allows threads and ISRs to pass arbitrary data items of variable or fixed size in first-in, first-out order.

Key properties of Zephyr FIFOs:
- **Zero-Copy Pointer Queue**: Data items are passed by reference (pointers).
- **Reserved Linkage Header**: Any structure enqueued in a `k_fifo` **must** have its first member reserved for kernel list management (`void *fifo_reserved`).
- **Thread Synchronization**: Pushing (`k_fifo_put`) wakes up waiting consumer threads, while reading (`k_fifo_get`) blocks efficiently until data is available.

---

## Directory Structure

```text
fifo/
├── CMakeLists.txt     # CMake build system rules
├── prj.conf           # Kconfig options (Logging, Timeslicing, Stack protection)
├── app.overlay        # Devicetree overlay (Default/empty)
├── README.md          # Topic documentation & bug analysis
└── src/
    └── main.c         # Producer-Consumer threads & FIFO queue application logic
```

---

## Analysis & Resolved Errors in Codebase

During code analysis of the `fifo` module, several compiler errors, logic flaws, and RTOS anti-patterns were identified in `src/main.c`:

| Component / Line | Issue / Anti-Pattern Identified | Impact | Corrective Action |
| :--- | :--- | :--- | :--- |
| `src/main.c:29` | **Missing `struct` Keyword**<br>`static my_struct *ptr = ...` | **Compilation Error**<br>`unknown type name 'my_struct'` | Change to `struct my_struct *ptr` or add a `typedef`. |
| `src/main.c:29` | **Static Local Variable in Loop**<br>`static struct my_struct *ptr = ...` inside `while(1)` | **Logic Flaw**<br>Initialized once at startup when `seq=0`. Never re-evaluates `seq % BUFFER` on loop iterations. | Remove `static` modifier so `ptr` evaluates on each loop iteration. |
| `src/main.c:29-34` | **Double-Enqueue / Linked List Corruption**<br>Re-enqueuing `&buffer_array[0]` repeatedly | **Kernel Crash / Hard Fault**<br>Zephyr overwrites `fifo_reserved` pointer of an active queue node, destroying list integrity. | Dynamically allocate items (`k_malloc`/`k_free`) or use semaphores to protect static slots. |
| `src/main.c:17,27-36` | **Unsynchronized Static Buffer Pool**<br>Cycling `buffer_array[seq % 4]` without tracking free slots | **Race Condition**<br>Producer overwrites and re-enqueues buffer slots still queued or in use by consumer. | Manage memory with `k_malloc()` & `k_free()` or switch to `k_msgq` for value-copy queues. |
| `src/main.c:51` | **Consumer Thread Delay Bottleneck**<br>`k_msleep(1000)` inside consumer after `k_fifo_get` | **Latency & Queue Backlog**<br>Unnecessary delay since `k_fifo_get(&my_fifo, K_FOREVER)` already blocks until data arrives. | Remove redundant `k_msleep(1000)` in consumer to process incoming items immediately. |

---

## Technical Details & Key APIs Used

### 1. Structure Definition
The first member of the data structure passed to `k_fifo` must be reserved for kernel list linkage:
```c
struct my_struct {
    void *fifo_reserved;  /* Reserved for Zephyr kernel FIFO list tracking */
    int id;               /* Payload data */
};
```

### 2. Static FIFO Initialization
```c
K_FIFO_DEFINE(my_fifo);
```

### 3. Producer: Enqueuing Data (`k_fifo_put`)
```c
struct my_struct *ptr = k_malloc(sizeof(struct my_struct));
if (ptr != NULL) {
    ptr->id = seq++;
    LOG_INF("PRODUCER PUSHED ID: %d", ptr->id);
    k_fifo_put(&my_fifo, ptr);
}
```

### 4. Consumer: Dequeuing Data (`k_fifo_get`)
```c
/* Blocks execution until an item is enqueued */
struct my_struct *rx_ptr = k_fifo_get(&my_fifo, K_FOREVER);
if (rx_ptr != NULL) {
    LOG_INF("CONSUMER RECEIVED ID: %d", rx_ptr->id);
    k_free(rx_ptr);  /* Release memory */
}
```

---

## How to Build and Run

From the root of your Zephyr RTOS workspace:

### 1. Build for QEMU Simulator
```bash
west build -b qemu_x86 fifo -p always
```

### 2. Run in Simulator
```bash
west build -t run
```

### 3. Build & Flash to Physical Target Board
```bash
west build -b <your_board_name> fifo
west flash
```

---

## Expected Output

Serial terminal / QEMU log output:

```text
*** Booting Zephyr OS build v3.5.0 ***
[00:00:00.000,000] <inf> app: BAVA.....
[00:00:00.000,000] <inf> app: ENTERING PRODUCER THREAD
[00:00:00.000,000] <inf> app: ENTERING CONSUMER THREAD
[00:00:00.000,000] <inf> app: PRODUCER PUSHED ID :1
[00:00:00.000,000] <inf> app: CONSUMER RECEIVED ID :1
[00:00:01.000,000] <inf> app: PRODUCER PUSHED ID :2
[00:00:01.000,000] <inf> app: CONSUMER RECEIVED ID :2
[00:00:02.000,000] <inf> app: PRODUCER PUSHED ID :3
[00:00:02.000,000] <inf> app: CONSUMER RECEIVED ID :3
```
