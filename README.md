# Zephyr RTOS Hands-On Tutorial & Topic-by-Topic Guide

Welcome to the **Zephyr RTOS Tutorial Series**! This repository provides a modular, topic-by-topic hands-on guide for learning and practicing **Zephyr Real-Time Operating System (RTOS)** concepts on embedded targets and simulators (e.g., QEMU).

---

## 📌 Repository Overview

Zephyr RTOS provides a powerful, modular architecture separating hardware specification (**Devicetree**), kernel feature configuration (**Kconfig**), and application logic (**Kernel C APIs**). This repository breaks down complex RTOS concepts into isolated, digestible modules to give developers practical experience.

---

## 🗺️ Topic Roadmap

The tutorial modules are organized sequentially, building from basic hardware abstraction to multithreading and kernel synchronization:

| Module Directory | Topic / Concept Focus | Key Zephyr Features & APIs |
| :--- | :--- | :--- |
| 📁 [firstapp](file:///home/bava/Desktop/ZephyrRTOS_Tutorial/firstapp) | **Basic Application & GPIO Control** | Devicetree overlays (`app.overlay`), GPIO driver (`gpio_pin_toggle_dt`), kernel sleep (`k_msleep`) |
| 📁 [hwins](file:///home/bava/Desktop/ZephyrRTOS_Tutorial/hwins) | **Hardware & Kconfig Inspector** | `IS_ENABLED()`, compile-time Kconfig queries (`CONFIG_ARM`, etc.), Devicetree property macros (`DT_PROP`), UART `printk` |
| 📁 [interrupt](file:///home/bava/Desktop/ZephyrRTOS_Tutorial/interrupt) | **GPIO Interrupts & Deferred Workqueue** | `gpio_pin_interrupt_configure_dt`, `gpio_add_callback`, `K_WORK_DELAYABLE_DEFINE`, `k_work_reschedule`, Software Debouncing |
| 📁 [threadeg](file:///home/bava/Desktop/ZephyrRTOS_Tutorial/threadeg) | **Multithreading & Thread Priorities** | Static thread creation (`K_THREAD_DEFINE`), Cooperative (negative priority) vs. Preemptive (positive priority) scheduling |
| 📁 [schedule](file:///home/bava/Desktop/ZephyrRTOS_Tutorial/schedule) | **Kernel Scheduler & Timeslicing** | `k_sched_lock()`, `k_sched_unlock()`, Round-Robin timeslicing (`CONFIG_TIMESLICING`), Zephyr Logger Subsystem |
| 📁 [sync](file:///home/bava/Desktop/ZephyrRTOS_Tutorial/sync) | **Synchronization & Inter-Thread IPC** | Semaphores (`k_sem`), Mutexes (`k_mutex`), Events (`k_event`), Message Queues (`k_msgq`) |
| 📁 [workqueue](file:///home/bava/Desktop/ZephyrRTOS_Tutorial/workqueue) | **System Workqueue** | `K_WORK_DEFINE`, `k_work_submit`, `CONFIG_SYSTEM_WORKQUEUE_STACK_SIZE` |
| 📁 [customq](file:///home/bava/Desktop/ZephyrRTOS_Tutorial/customq) | **Custom User Workqueue** | `k_work_queue_init`, `k_work_queue_start`, `k_work_submit_to_queue`, `CONTAINER_OF` |
| 📁 [fifo](file:///home/bava/Desktop/ZephyrRTOS_Tutorial/fifo) | **First-In, First-Out Queue IPC** | `K_FIFO_DEFINE`, `k_fifo_put`, `k_fifo_get`, Producer-Consumer multithreading |
| 📁 [memslab](file:///home/bava/Desktop/ZephyrRTOS_Tutorial/memslab) | **Memory Slab Allocation & IPC** | `K_MEM_SLAB_DEFINE`, `k_mem_slab_alloc`, `k_mem_slab_free`, `k_fifo` zero-copy messaging |
| 📁 [uart](file:///home/bava/Desktop/ZephyrRTOS_Tutorial/uart) | **Interrupt-Driven UART & CLI Parsing** | `uart_irq_callback_set`, `ring_buf_put`, `k_sem`, `strcasecmp` CLI command parser |
| 📁 [i2c](file:///home/bava/Desktop/ZephyrRTOS_Tutorial/i2c) | **I2C Bus & Sensor Communication** | `i2c_dt_spec`, `i2c_write_read_dt`, `i2c_is_ready_dt`, `k_mutex` thread safety |

---

## 🚀 Scalable Architecture for Future Topics

This repository is designed with a **standardized, scalable folder structure**. New RTOS topics can easily be added as standalone modules without affecting existing applications.

### Future Expansion Topics (Planned / Extensible)
- ⏱️ **Timers & Workqueues**: Hardware timers (`k_timer`), system workqueues (`k_work`), delayed work routines.
- ⚡ **Zero-Latency Interrupts**: Direct & Zero-latency ISR handling, nested interrupts.
- 💾 **Advanced Memory Management**: Heap memory allocation (`k_malloc`), Mailboxes (`k_mbox`).
- 📡 **Hardware Communication Drivers**: UART interrupts, I2C sensor drivers, SPI communication.
- 🌐 **Advanced Networking & Wireless**: BLE (Bluetooth Low Energy), TCP/IP stack integration.

---

## 🏗️ Standard Module Anatomy

Every module folder in this repository follows the standard Zephyr application structure:

```text
<topic_directory>/
├── CMakeLists.txt      # Build rules & Zephyr package inclusion
├── prj.conf            # Kconfig options enabling kernel/driver features
├── app.overlay         # Devicetree overlay for hardware pin mapping
├── README.md           # Topic-specific documentation & walk-through
└── src/
    └── main.c          # Application source code
```

---

## 🛠️ Prerequisites & Setup

Before building the tutorial examples, ensure you have set up the Zephyr development environment:

1. **Zephyr SDK & Dependencies**: Install Python 3, `west`, CMake, Ninja, and the Zephyr SDK cross-compiler toolchain.
2. **Initialize Workspace**:
   ```bash
   # Initialize west workspace (if not already set up)
   west init -m https://github.com/zephyrproject-rtos/zephyr --mr v3.5.0 zephyrproject
   cd zephyrproject
   west update
   west zephyr-export
   ```
3. **Environment Variables**:
   ```bash
   source zephyr/zephyr-env.sh
   ```

---

## ⚡ How to Build & Run Any Module

To compile and flash any specific module (e.g., `firstapp`), navigate into the root of this workspace and execute `west build`:

### 1. Build for a Physical Target (e.g., STM32 / NXP / ESP32)
```bash
west build -b <your_board_name> <topic_directory>
# Example:
west build -b stm32f4_disco firstapp
```

### 2. Build for QEMU Simulator (No Hardware Required)
```bash
west build -b qemu_cortex_m3 firstapp
west build -t run
```

### 3. Flash to Board
```bash
west flash
```

---

## ➕ Adding a New Topic Directory

To add a new topic module to this repository:
1. Create a new directory (e.g., `workqueue/`).
2. Add `CMakeLists.txt`, `prj.conf`, `app.overlay`, and `src/main.c`.
3. Create a topic-specific `README.md` following the standard template used in this repository.
4. Update the **Topic Roadmap** in this root `README.md`.
