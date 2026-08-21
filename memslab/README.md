# Zephyr RTOS Memory Slab Demo (`memslab`)

This project demonstrates **Fixed-Size Memory Pool Management** using Zephyr RTOS's **Kernel Memory Slab (`k_mem_slab`) API**, integrated with a **FIFO Queue (`k_fifo`)** for zero-copy, non-fragmenting Producer-Consumer multithreading.

---

## Overview

In real-time embedded systems, dynamic heap allocation (`malloc`/`free`) can lead to heap fragmentation, non-deterministic allocation delays, and out-of-memory faults. Zephyr RTOS provides **Memory Slabs (`k_mem_slab`)** as a deterministic, fixed-size memory pool mechanism:
- **O(1) Deterministic Allocation**: Allocating and freeing memory blocks happens in constant time.
- **Zero Memory Fragmentation**: All blocks within a slab pool are identical in size.
- **Zero-Copy IPC Integration**: Memory blocks can be allocated dynamically by a producer, enqueued into a `k_fifo`, processed by a consumer, and returned back to the slab pool without copying data buffers.

---

## Directory Structure

```text
memslab/
├── CMakeLists.txt     # CMake build system rules
├── prj.conf           # Kconfig options (Logging, Thread naming, Stack protection, Timeslicing)
├── app.overlay        # Devicetree overlay (Default/empty)
├── README.md          # Topic documentation & bug analysis
└── src/
    └── main.c         # Memory slab definition, Producer-Consumer threads & FIFO logic
```

---

## Analysis & Resolved Code Errors

During code analysis of `src/main.c` and build scripts in `memslab/`, several key aspects and a critical C pointer indirection bug were identified:

| Component / Line | Code Snippet / Issue | Impact | Corrective Action |
| :--- | :--- | :--- | :--- |
| `src/main.c:47` | **Pointer Indirection Bug**<br>`k_mem_slab_free(&var, (void *)&rx);` | **Memory Corruption / Kernel Fault**<br>Passes `&rx` (the address of the local stack pointer variable) instead of `rx` (the pointer to the memory slab block). The kernel tries to free a stack address. | Change `(void *)&rx` to `rx` or `(void *)rx`. |
| `src/main.c:12` | `K_MEM_SLAB_DEFINE(var, sizeof(struct fifo_struct), 10, 4);` | **Memory Pool Setup**<br>Creates a pool of 10 blocks, each `sizeof(struct fifo_struct)` bytes, aligned to a 4-byte (32-bit) boundary. | Correct usage. |
| `src/main.c:6-10` | `void *fifo_reserved;` as first struct member | **Required Linkage Header**<br>`k_fifo` requires the first member of any enqueued structure to be a reserved kernel pointer. | Correct usage for `k_fifo` integration. |
| `CMakeLists.txt:6` | `project(zephyr_interrupt_demo)` | **Minor Copy-Paste Artifact**<br>Project name is set to `zephyr_interrupt_demo` instead of `zephyr_memslab_demo`. | Rename to `project(zephyr_memslab_demo)`. |

---

## Technical Details & Key APIs Used

### 1. Memory Slab Definition
```c
/* Defines a memory slab named 'var' with 10 blocks, aligned to 4 bytes */
K_MEM_SLAB_DEFINE(var, sizeof(struct fifo_struct), 10, 4);
```

### 2. Allocating a Block (`k_mem_slab_alloc`)
```c
struct fifo_struct *ptr;

/* Allocates a block from memory slab 'var'. Blocks indefinitely if empty */
if (!k_mem_slab_alloc(&var, (void **)&ptr, K_FOREVER)) {
    ptr->id = seq++;
    k_fifo_put(&my_fifo, ptr);  /* Enqueue pointer to FIFO */
    LOG_INF("PRODUCER PUSHED ID :%d", ptr->id);
}
```

### 3. Dequeuing & Freeing Block (`k_mem_slab_free`)
```c
struct fifo_struct *rx = k_fifo_get(&my_fifo, K_FOREVER);

if (rx != NULL) {
    LOG_INF("CONSUMER GOT ID :%d", rx->id);
    
    /* Return the memory block to the slab pool */
    k_mem_slab_free(&var, rx);
}
```

---

## Kconfig Configuration (`prj.conf`)

- `CONFIG_LOG=y`: Enables the Zephyr Logging subsystem.
- `CONFIG_LOG_DEFAULT_LEVEL=3`: Sets log level to `LOG_LEVEL_INF`.
- `CONFIG_THREAD_NAME=y`: Enables thread naming for debugging.
- `CONFIG_HW_STACK_PROTECTION=y`: Enables hardware stack overflow detection.
- `CONFIG_TIMESLICING=y`: Enables cooperative/preemptive thread timeslicing.

---

## How to Build and Run

From the root of your Zephyr RTOS workspace:

### 1. Build for QEMU Simulator
```bash
west build -b qemu_cortex_m3 memslab -p always
```

### 2. Run in Simulator
```bash
west build -t run
```

### 3. Build & Flash to Physical Target Board
```bash
west build -b <your_board_name> memslab
west flash
```

---

## Expected Output

Serial terminal / QEMU console output:

```text
*** Booting Zephyr OS build v3.5.0 ***
[00:00:00.000,000] <inf> app: MEMORY SLAB Tutorial with FiFo
[00:00:00.000,000] <inf> app: ENTERING PRODUCER THREAD
[00:00:00.000,000] <inf> app: ENTERING CONSUMER THREAD
[00:00:00.000,000] <inf> app: PRODUCER PUSHED ID :0
[00:00:00.000,000] <inf> app: CONSUMER GOT ID :0
[00:00:01.000,000] <inf> app: PRODUCER PUSHED ID :1
[00:00:01.000,000] <inf> app: CONSUMER GOT ID :1
[00:00:02.000,000] <inf> app: PRODUCER PUSHED ID :2
[00:00:02.000,000] <inf> app: CONSUMER GOT ID :2
```
