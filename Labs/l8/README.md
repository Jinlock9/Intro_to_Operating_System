# l8

## ECE4820 Tasks

### Looking at the Linux kernel

#### What are `kmsan`, `kasan`, and `kfence` folders?

- **KMSAN (Kernel Memory Sanitizer)**: Detects uses of uninitialized memory within the kernel.

    - mm/kmsan/

- **KASAN (Kernel Address Sanitizer)**: Identifies out-of-bounds and use-after-free errors in kernel memory.

    - mm/kasan/

- **KFENCE (Kernel Electric-Fence)**: A low-overhead, sampling-based detector for heap out-of-bounds access, use-after-free, and invalid-free errors, designed for production environments.

    - mm/kfence/

#### What is *shadow memory* and how does it relate to sanitizers?

**Shadow memory** is a mechanism used by sanitizers to track metadata about program memory, such as whether it is initialized, valid, or accessible. Each byte of shadow memory corresponds to a portion of program memory and is update during execution to detect issues like:

- **KMSAN**: Tracks uninitialized memory.
- **KASAN**: Detects out-of-bounds and use-after-free errors.
- **KFENCE**: Uses lightweight sampling to catch similar errors with minimal overhead.

#### What are sanitizers and why are they essential when coding in the kernel level?

**Sanitizers** are debugging tools integrated into software systems (such as the Linux kernel) to detect and report runtime errors like memory corruption, invalid accesses, and data race conditions. They are essential at kernel level because of the kernel's critical role in system stability, security, and performance.

- Why Sanitizers are Essential when coding in the kernel level?

    1. Kernel Bugs are Critical:

        A single bug in the kernel can cause system crashes, security vulnerabilities, or data corruption. Sanitizers catch bugs early.

    2. Memory Safety is Complex:

        The kernel deals with low-level memory management, making it prone to: *Use-after-free*, *Buffer overflows*, and *Uninitialized memory*.

    3. Concurrency Issues:

        Kernels are highly multi-threaded. Sanitizers help detect *data races* and *deadlocks*.

    4. Debugging is Harder in Kernels:

        Kernel code lacks the rich debugging tools available for user-space programs. Sanitizers provide insights into memoru and thread issues that are hard to diagnose otherwise.

#### What is `kmemleak.c` file about? Explain who should "use" it and under what circumstances.

The `kmemleak.c` file in the Linux kernel source code implements **Kernel Memory Leak Detector (kmemleak)**, a runtime tool designed to detect memory leaks in the kernel.

- What is `kmemleak.c`?
    - It provides an infrastructure to monitor dynamic memory allocations in the kernel (e.g., `kmalloc`, `vmalloc`, etc.).
    - It uses a garbage-collection-like approach to identify memory allocations that are no longer referenced but not freed.
    - `kmemleak` maintains a metadata structure for every tracked memory block and periodically scans memory to check for reachable allocations. Unreachable allocations are flagged as leaks.

- When and why to use `kmemleak`?
    - During Development:
        - To catch memory leaks early in the development lifecycle, ensuring robust and memory-efficient code.
        - Particularly helpful in identifying issues in new or modified subsystems.
    - Debugging Production Issues:
        - In controlled test environments when debugging memory-related issues in production kernels.
        - Can be used to investigate unexpected memory growth (e.g., when running low on kernel memory).
    - Stress Testing:
        - To validate the stability of kernel code under high memory pressure or extended runtime.

---

#### `nommu.c`: when this file be used? List some drawbacks of having CPU without any MMU.

The `nommu.c` file in the Linux kernel is part of the **No Memory Management Unit** support. It is used in systems where the CPU lacks a **Memory Management Unit (MMU)**, which means there is no hardware-based virtual memory or memory protection.

- When is `nommu.c` used?
    - **Embedded Systems**: Many low-power, resource-constrained devices use CPUs without an MMU to reduce cost and complexity.
    - **Real-Time Systems**: Some real-time operating systems perfe NoMMU systems to avoid MMU overhead and achieve predictable performance.
    - **Specialized Architecture**: For CPUs designed for specific use cases.  

- Drawbacks of CPUs without MMU:
    1. **No Virtual Memory**: Applications run directly in physical memory, limiting features like *memory isolation* and *demand paging*.
    2. **Lack of Memory Protection**: Programs can access or overwrite each other's memory, making the system vulnerable to accidental or malicious memory corruption.
    3. **Limited Multitasking**:
        - Without virtual memory, switching between processes is harder, as each process must manage its memory directly.
        - Context switching overhead increases because memory layouts are fixed and cannot be dynamically adjusted.
    4. **No Address Space Layout Randomization (ASLR)**: ASLR, a key security feature, relies on MMU support to randomize memory locations, making NoMMU, systems more vulnerable to exploits.

#### `compaction.c`: what is this file used for? Why is "memory compaction" important?

The `compaction.c` file in the Linux kernel handles **memory compaction**, which is the process of rearranging memory pages to create larger contiguous blocks of free memory. This feature is primarily used to support memory allocation for large pages and to avoid memory fragmentation.

- What is `compaction.c` used for?

    - It implements algorithms for:
        - Moving in-use memory pages to one side of the momory pool.
        - Consolidating free pages on the other side.

    - This enables the kernel to allocate large contiguous memory blocks, which are critical for features like:
        - **HugePages**: Improves performance by reducing Translation Lookaside Buffer (TLB) misses.
        - **DMA (Direct Memory Access)**: Some devices require physically contiguous memory for efficient data transfer.

- Why is Memory Compaction Important?

    1. Reduces Memory Fragmentation:

        Over time, memory becomes fragmented as processes allocate and deallocate memory of various sizes. Compaction rearranges memory to address this fragmentations.

    2. Supports HugePages:

        *HugePages* reduce the overhead of managing many small pages by using fewer, larger pages. Memory compaction is essential for allocating these larger pages when contiguous memory is not available.

    3. Improves System Performance:

        Contiguous memory allocations reduce CPU overhead in managing scattered memory pages, improving overall system performance.

---

### `oom_kill.c`
#### What is the OOM killer? In particular what does OOM mean?

The **Out-Of-Memory (OOM) Killer** is a mechanism in the Linux kernel designed to resolve situations where the system runs out of memory (OOM). When all available memory (including swap space) is exhausted, the kernel cannot allocate more memory for running processes, potentially leading to a system hang or crash. The OOM killer is invoked to terminate one or more processes to free up memory and keep the system functional.  

**OOM** stands for **Out of Memory**, a situation where the system has exhausted all its available memory resources, including physical RAM and swap space.  

#### Based on the source code and comments, explain what could be a reason for a deadlock in the Linux kernel?
1. **Locking Issues**: Circular dependencies or inconsistent lock acqusition order.
2. **Blocked Victim Process**: The OOM killer selects a process in an uninterruptible state, holding critical resources.
3. **Memory Starvation**: The OOM killer fails due to insufficient kernel memory to operate.
4. **Faulty Victim Selection**: Choosing unkillable processes or kernel threads.
5. **Race Conditions**: Concurrent execution of the OOM killer causing inconsistencies.

---

#### The "rough" layout of kernel and user space memories.
In a typical Linux system, the memory is divided into **user space** and **kernel space**:

- **User Space Memory**:
    - Accessible by User Applications: This area is used by user-level processes.
    - Virtual Address Range: In a 64-bit systems, it occupies the lower part of the virtual address space.
        - Example: `0x0000000000000000` to `0x00007FFFFFFFFFFF`
    - Protection: User space is protected; processes cannot access kernel space directly to ensure stability and security.
    - Swappable: User space memory can be swapped to disk to free up physical RAM.

- **Kernel Space Memory**:
    - Reserved for Kernel Operations: Used by the kernel and its modules, including device drivers and memory management structures.
    - Virtual Address Range: Occupies the upper part of the virtual address space.
        - Example: `0xFFFF800000000000` to `0xFFFFFFFFFFFFFFFF`
    - Not Directly Accessible by Users: Usser processes cannot directly access kernel space memory.

#### Whether or not kernel memory can be swapped and why?
**Kernel Memory cannot be swapped** because:
- Kernel memory holds essential structures like process control blocks and page tables. Swapping it could cause system failures.
- Real-Time Access: Kernel operations need immediate access to memory; swapping would introduce delays.
- The kernel manages swapping, and swapping its own memory could lead to deadlocks or inconsistencies.
- Kernel Memory is crucial during low-memory conditions, making its swapping counterproductive.
---

## ECE4821 Tasks

### More on MM
#### What is the difference between `vmalloc` and `kmalloc`? Which one is it often best to use?
- `kmalloc`: Allocates physically contiguous memory, fast, suitable for small allocations, and works well for DMA and performance-critical tasks.
- `vmalloc`: Allocates virtually contiguous but physically scattered memory, slower, suitable for large allocations where physical contiguity is unnecessary.

- **Best to Use**:
    - `kmalloc` for better performance and simpler usage
    - `vmalloc` for large, non-critical allocations

#### What is `/dev/shm`? Explain a simple use case.
`/dev/shm` is a **temporary file storage filesystem** backed by **RAM**, provided by the Linux kernel. It is typically implemented using **tmpfs**, a memory-based filesystme.

- Simple Use Case  
    - **Inter-Process Communication (IPC)**
        Processes can use /dev/shm for fast, shared memory communication. For example:

        1. Process A writes data to a shared memory file in /dev/shm.
        2. Process B reads the same file to process the data.


#### What is page poisoning and when could it be useful? (`page_poison.c`)
**Page poisoning** is a Linux kernel debugging feature that detects memory misuse by filling freed or uninitialized memory pages with a predefined "poison" pattern. It helps catch bugs like use-after-free, uninitialized access, and double-free errors.

- When Could Page Poisoning Be Useful?
    - Kernel Development: Detects use-after-free and memory corruption bugs.
    - Driver Development: Ensures proper memory handling in drivers.
    - Security Testing: Identifies vulnerabilities like buffer overflows.
    - Memory Validation: Verifies correct memory management behavior.
    - Rare Bug Testing: Surfaces subtle, hard-to-reproduce memory issues.


#### Where is the page table structure defined?
The page table structures are defined in architecture-specific files, typically under `arch/<arch>/include/asm/pgtable.h`. For x86, these structures and related macros are found in `arch/x86/include/asm/pgtable.h` and `pgtable_types.h`. Common abstractions are in `include/linux/mm_types.h` and `include/linux/pgtable.h`.

#### Why should the memory be handled differently at boot and run times? (`memblock.c`)
Memory must be handled differently at boot and runtime because boot-time allocations rely on simple mechanisms like `memblock` due to limited system initialization, while runtime memory handling uses advanced allocators to support dynamic, efficient, and complex memory operations for both the kernel and user-space applications.