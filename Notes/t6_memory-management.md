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

### Initializing the Swap Area
![Initializing the swap area](img/t6_4.png)

- Two main strategies:
    - Copy the whole process image to the swap area
    - Allocate swap disk space on the fly

### Memory Management Unit
![MMU](img/t6_5.png)

- Very low-level hardware memory management.
- Fast but won't be able to do much

- mm, arch

## Paging 
### Virtual Page and Page Frame 
![Virtual page and page frame](img/t6_6.png)

#### Organizing the memory:
- Divide the virtual address space into fixed-size units called *pages*.
- Pages and *page frames* are usually of same size
- MMU maps virtual addresses to physical addresses
- MMU causes the CPU to trap on a page fault
- OS copies content of a little used page onto the disk
- Page frame loaded onto newly freed page

### Page Table
![Page Table](img/t6_7.png)

#### Structure of a page entry:
- Present|absent : 1|0; missing causes a page fault
- Protection: 1 to 3 bits: reading/writing/executing
- Modified: 1|0 = dirty|clean; page was modified and needs to updated on the disk
- Referenced: bit used to keep track of most used pages; useful in case a page fault
- Caching: important for pages that map to registers; do not want to use old copy so set caching to 0

### Paging 
#### Two main issues must be solved in a paging systme:
- Mapping must be done efficiently
- A large virtual address space implies a large page table
#### Translation Lookaside Buffer (TLB):
- Hardware solution implemented inside the MMU
- Keeps track of few most used pages
- Features the same fields as for page table entries including the virtual page number and page frame.

### Page Replacement
On a **page fault** the following operations are performed:
- Choose a page to remove from the memory
- If the page was modified while in the memory, it needs to be rewritten on the disk; otherwise nothing needs to be done
- Overwrite the page with the new memory content

*How to optimize the selection of the page to be evicted?*

### Page Replacement - Optimal Solution
Determining which page to remove when a **page fault** occurs:
- Label and order all the pages in memory
- Pages with lower labels are used first
- Pages with larger label are swapped out of the memory

*Can the information be known ahead of time?*

### Page Replacement - LRU
*Recently heavily used pages are very likely to be used again soon*

#### Hardware solution, for *n* page frames:
- Initialize a binary *n x n* matrix to 0
- When a frame *k* is used set row *k* to 1 and column *k* to 0
- Replace the page with the smallest value

![LRU](img/t6_8.png)

### Page Replacement - Aging
- Simulating LRU in software:
    - For each page initialize an *n*-bit software counter to 0
    - At each clock interrupt, the OS scans all the pages in memory
    - Shift all the counters by 1 bit to the right
    - Add $2^{n-1}$. *R* to the counter.

- Example. *n* = 8 with 4 pages over 4 clock interrupts

![Aging](img/t6_9.png)


### Definitions
Basic notions related to **paging**:
- Demand paging: pages are loaded on demand
- Locality reference: during an execution phase, a process only accesses a small fraction of all its pages
- Working set: set of pages currently used by a process
- Thrashing: process causes many page fault due to a lack of memory
- Pre-paging: pages loaded in memory before letting process run
- Current virtual time: amount of time during which a process has used to CPU
- t (tow): a constant representing the age of the working set

### Page Replacement - WSClock
The **WSClock (Working Set Clock)** algorithm is a **page replacement strategy** that combines the concepts of the **working set model** and the **clock replacement algorithm**. It efficiently determines which pages to replace based on the page's recency of use and their necessity in the current working set.

#### Algorithm Steps
1. Data Structures:
    - Each page has the following attributes:
        - **Reference Bit (R):**: Indicate if the page was accessed recently.
        - **Age**: Timestamp of the page's last access.
        - **Modified Bit (M)**: Indicates if the page is dirty (modified).

2. Clock Hand Traversal:
    - The algorithm maintains a circular list of pages.
    - The clock hand sweeps through the list, checking one page at a time.

3. **Page Evaluation**: For each page, the algorithm evaluates based on the following conditions:
    - Case 1: **Page is in the working set ($R = 1$ and $age >= τ$)**:
        - Reset $R$ to 0 and move to the next page.
        - This means the page is still needed and remains in memory.
    - Case 2: **Page is not in the working set ($R = 0$ and $age > τ$)**:
        - Check $M$:
            - If $M = 1$ (dirty page): Schedule the page to be written to disk and continue searching.
            - If $M = 0$ (clean page): Replace the page immediately.
    - Case 3: **Page is clean but not in the working set ($R = 0 and M = 0$)**:
        - Replace the page immediately.

4. Repeat Search if Necessary:

    If non suitable page is found in one complete sweep, the algorithm writes a dirty page to disk and replaces it.

![WSClock](img/t6_10.png)

### Local vs. Global Allocation
Onto which set should the page replacement algorithm be applied:
- Local, i.e. within the process:
    - Allocate a portion of the whole memory to a process
    - Only use the allocated portion
    - Number of page frames for a process remains constant
- Global, i.e. within the whole memory:
    - Dynamically allocate page frames to a processes
    - Number of page frames for a process varies over time

*Which approach is best?*

#### Local Allocation
Each process is allocated a fixed number of page frames, and the page replacement algorithm operates only within those frames.

- Advantages:
    - Isolation: A process cannot negatively impact others by monopolizing page frames. This isolation prevents a single poorly-behaved process from degrading the entire system.
    - Predictability: The performance of each process is more predictable since the number of page frames allocated remains constant.
    - Simpler Debugging: Performance issues are easier to trace because they are confined to individual processes.
- Disadvantages:
    - Inefficient Memory Utilization: Some processes may not fully utilize their allocated frames, while others may suffer from insufficient allocation.
    - Difficult Allocation Decisions: Determining the right number of frames for each process is non-trivial and may lead to under- or over-provisioning.

#### Global Allocation
Page frames are allocated dynamically across all processes. The page replacement algorithm operates over the entire memory space, reallocating frames as needed.

- Advantages:
    - Efficient Use of Memory: Idle processes release frames for use by active ones, leading to better overall memory utilization.
    - Improved Performance: Active processes can dynamically acquire more frames to meet their needs, reducing the likelihood of page faults.
    - Flexibility: The system can adapt to changing workloads without manual intervention.
- Disadvantages:
    - Process Interference: A process with high memory demand can starve others, leading to thrashing and performance degradation.
    - Unpredictability: Performance is harder to guarantee because a process's page frame allocation can vary widely over time.
    - Complexity: Managing and monitoring dynamic allocation adds overhead to the system.

#### Which is Best?
- **Local Allocation** is better when:
    - The system needs isolation between processes.
    - Processes have predictable memory demands.
    - Stability and fairness are critical.
- **Global Allocation** is better when:
    - The system aims for **maximum efficiency** and can tolerate unpredictability.
    - Workloads vary significantly, with some processes requiring much more memory than others.
    - There's a need to dynamically balance memory across all processes.

#### Practical Considerations
- Many modern systems use **global allocation** for efficieny but incorporate fairness mechanisms to prevent starvation (e.g., working set models or proportional allocation).
- A hybrid approach is possible, where a base set for frames is allocated locally, and additional frames are dynamically allocated globally.

### Page Fault Frequency
Adjusting the number of pages:
- Start process with a number of pages proportional to its size.
- Adjust page allocation based on the page fault frequency
    - Count number of page fault per second
    - If larger than *A* then allocate more page frames
    - If below *B* then free some page frames

![Page Fault Frequency](img/t6_11.png)

### Page Size
Finding optimal page size given a page frame size:
- In average, half of the last page is used (internal fragmentation)
- The smaller the page size, the larger the page table

- Page size *p*, process size *s* bytes, average size for page entry *e* and overhead *o*:  
    
    **$o = se/p+p/2$**  

- Differentiate with respect to *p* and equate to 0:  

    **$1/2=se/p^2$**

- Optimal page size:

    **$p = root(2se)$**

- Common page frame sizes:

    **4KB or 8KB**


### Page Sharing 
#### Decrease memory usage by *sharing pages*:
- Pages containing the program can be shared.
- Personal data should not be shared.

![Page Sharing](img/t6_12.png)

#### Several basic problems arise:
- On a process switch, do not remove all pages if required by another process: would generate many page fault.
- When a process terminates, do not free all the memory if it is required by another process: would generate a crash

- How to share data in *read-write* mode?
    - **Copy-on-Write (CoW)**:
        - Pages are initially shared as **read-only**.
        - If a process attempts to modify a shared page, the **operating system creates a private copy** of the page for that process.
        - This ensures that modifications by one process do not affect the other, maintaining data consistency.
        - Steps:
            - Mark the shared page as **read-only**
            - On a write attempt, trigger a **page fault**.
            - Allocate a new page, copy the contents of the shared page, and assign it to the writing process.
    - **Memory-Mapped Files**:
        - Use **memory-mapped files** to map a file into the address space of multiple processes.
        - The file acts as a shared medium for data, and changes are reflected across all process.
        - To implement:
            - Use system calls like `mmap` to map a file into memory.
            - Ensure file permissions are set appropriately to allow read-write access.
    - **Shared Memory Segments**:
        - Explicitly create shared memory regions using APIs such as:
            - `shmget` (System V shared memory) or `mmap` (POSIX shared memory).
        - Each process attaches to the shared memory segment and can read/write data.
        - Synchronization is critical to prevent data races:
            - Use **semaphores**, **mutexes** or other locking mechanism.

### Importance of paging in the OS
OS involved in paging related work on *four* occasions:

1. Process Creation:
    - Determine process size
    - Create process' page table (allocate and initialize memory)
    - Initialize swap area
    - Store information related to the swap area and page table in the process

2. Process Execution:
    - MMU resets for the new process
    - Flush the TLB
    - Make the new process' page table the current one

3. Page Fault:
    - Read hardware register to determine origin of page fault
    - Compute which page is needed
    - Locate the page on the disk
    - Find an available page frame and replace its content
    - Read the new page frame
    - Rewind to the faulting instruction and re-execute it

4. Process Termination:
    - Release page table entries, pages, and disk space
    - Beware of any page that could be shared among several processes.

### Page Fault Handling
#### Process on a page fault:
1. Trap to the kernel is issued; program counter is saved on the stack; state of current instruction saved on some specific registers.
2. Assembly code routine started: save general registers and other volatile information.
3. OS search which page is requested.
4. Once the page is found: check if the address is valid and if process is allowed to access the page. If not kill the process; otherwise find a free page frame.
5. If selected frame is dirty: have a context switch (faulting process is suspended) until disk transfer has completed. The page frame is marked as reserved such as not to be used by another process.
6. When page frame is clean: schedule disk write to swap in the page. In the meantime the faulting process is suspended and other processes can be scheduled.
7. When receiving a disk interrupt to indicate copy is done: page table is updated and frame is marked as being in a normal state.
8. Rewind program to the faulting instruction, program counter reset to this value.
9. Faulting process scheduled.
10. Assembly code routine starts: reload registers and other volatile information.
11. Process execution can continue.

### Policy and Mechanism
Example showing how to dissociate policies from mechanisms:
- Low level MMU handler: architecture dependent
- Page fault handler: kernel space
- External handler: user space

![Policy and mechanism](img/t6_13.png)

### Organizational Issue
#### *Where should the page replacement algorithm go?*
- User Space:
    - Use some mechanism to access the R and M bits
    - Clean solution
    - Overhead resulting from crossing user-kernel boundary several times
    - Modular code, more flexibility
- Kernel Space:
    - Fault handler sends all information to external pager (which page was selected for removal)
    - External pager writes the page to the disk
    - No overhead, faster







