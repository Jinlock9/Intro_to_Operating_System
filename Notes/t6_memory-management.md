# Memory Management

## Handling Memory
### Memory Hierarchy
![Memory Hierarchy](img/t6_1.png)

#### Problems related to memory:
- From expensive to cheap, fast to slow
- Job of the OS to handle the memory
    - How to model the hierarchy?
    - How to manage this abstraction?

### Memory Manager
#### Efficiently manage memory:
- Keep track of which part of the memory is used.
- Allocate memory to processes when required
- Deallocate memory at the end of a process

*Remark*. It is the job of the hardware to manage the lowest levels of cache memory.

### Simplest Model
#### No memory abstraction:
- Program sees the actual physical memory
- Programmers can access the whole memory
- Limitations when running more than one program:
    - Have to copy the whole content of the memory into a file when switching program
    - No more than one program in the memory at a time
    - More than one program is possible if using special hardware

### Address Space
#### *No abstraction* leads to two main problems:
1. Protection: prevent program from accessing other's memory
    - Without abstraction, every program can access any part of the physical memory.
    - Risks:
        - A program can accidentally or maliciously overwrite another program's memory or even the OS's memory.
        - It leads to system instability, crashes, or security vulnerabilities.
2. Relocation: rewrite address to allocate personal memory
    - Programs often need to run at specific memory locations
    - If multiple programs are loaded into memory, their addresses may conflict.
    - Manually rewriting addresses for each program to avoid conflicts is error-prone and impractical.

#### A solution is to set an *address space*:
1. Definition of **Address Space**:
    - An address space is a logical view of memory assigned to a process.
    - It represents the range of memory addresses a process can use.
    - The address space is **independent of the physical memory**.
2. Benefits:
    - Protection:
        - Each process has its own isolated address space.
        - One process cannot directly access or interfere with another's process's memory.
        - This isolation is enforced by hardware (e.g., Memory Management Unit, MMU).
    - Relocation:
        - The OS and hardware dynamically map a process's logical addresses (virtual addresses) to physical memory.
        - Processes are written using logical addresses, and the MMU transparently handles translation to physical addresses at runtime.

### Memory Size Limitation
![Memory Size Limitation](img/t6_2.png)

#### When booting many processes are started:
- As more programs are run, more and more memory is needed.
- More memory than available might be needed.
- Processes are swapped in (out) from (to) the disk.
- OS has to manage dynamically assigned memory.

### Bitmap and linked lists
![Bitmap and linked lists](img/t6_3.png)

#### Simple idea:
- Define some base size for an area *s*
- Split up the whole memory into *n* chunks of size *s*
- Keep track of the memory used is a bitmap or linked list

- **Bitmaps** are better suited for systems with fixed-sized chunks and relatively uniform memory usage patterns.
- **Linked lists** are more effective for systems with variable-sized allocations, as they offer more flexibility but at the cost of complexity.

### Allocating Memory
#### Ways to assign memory to processes:
1. First Fit: search of a hole big enough and use the first found
2. Best Fit: search whole list and use smallest, big enough hole
3. Quick Fit: maintain lists for common memory sizes, use the best

#### Characteristics:
- Speed: quick fit > first fit > best fit
- Locally optimal: quick fit = best fit > first fit
- Globally optimal: first fit > quick fit = best fit

### Basic Idea
#### Virtual Memory:
**Virtual memory** is a memory management technique that allows processes to use more memory than is physically available by abstracting the physical memory into a larger, logical address space.

- Generalization of the base and limit registers
    - In simpler systems, each process has a **base register** (indicating the starting address of its allocated memory) and a **limit register** (indicating the size of the memory segment).
- Each process has its own address space
    - Each process operates in its **own private address space**:
        - The process perceives this space as contiguous and unlimited.
        - The OS and hardware manage how virtual addresses map to physical addresses.
- The address space is split into chunks called *pages*
- Each page corresponds to a range of addresses
- Pages are mapped onto physical memory
    - Pages in the virtual address space are **mapped** to **frames** in physical memory using a **page table**.
        - A **page table** keeps track of where each virtual page is located in physical memory.
        - Some pages may not be in physical memory at all and are stored in secondary storage (e.g., swap space on the disk).
- Pages can be on different medium, e.g. RAM and swap
    - Virtual memory allows pages to reside in:
        - **RAM** (fast, for frequently accessed pages).
        - **Swap space** (on a disk, used as overflow storage when RAM is full).
    - When a process accesses a page that is in swap space (not in RAM), a **page fault** occurs:
        - The OS retrieves the page from the disk and loads it into RAM.
        - This mechanism extends the system's effective memory capacity but can slow performance if disk access is frequent.

### The Swap Area
#### Swap partition principles:
- Simple way to allocate page space on the disk
- OS boots, swap is empty and defined by two numbers: its origin and its size
- When a process starts, its swap area is reserved and initialized.
- The new "origin" is computed.
- When a process terminates, its swap area is freed.
- The swap is handled as a list of free chunks.