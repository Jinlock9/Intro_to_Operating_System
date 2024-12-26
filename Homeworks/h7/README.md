# h7
## ECE4820 Exercises
### Ex. 1 Page Replacement Algorithm

In this exercise, we consider the WSClock page replacement algorithm with a τ value of two ticks. The system state is given as follows.

| Page | Time stamp | `Present` | `Referenced` | `Modified` |
|:----:|:----------:|:---------:|:------------:|:----------:|
| 0    | 6          | 1         | 0            | 1          |
| 1    | 9          | 1         | 1            | 0          |
| 2    | 9          | 1         | 1            | 1          |
| 3    | 7          | 1         | 0            | 0          |
| 4    | 4          | 0         | 0            | 0          |

#### 1. Explain the content of the new table entries if a clock interrupt occurs at tick 10.

| Page | Time stamp | `Present` | `Referenced` | `Modified` |
|:----:|:----------:|:---------:|:------------:|:----------:|
| 0    | 6          | 1         | 0            | 1          |
| 1    | 9          | 1         | 0            | 0          |
| 2    | 9          | 1         | 0            | 1          |
| 3    | 7          | 1         | 0            | 0          |
| 4    | 4          | 0         | 0            | 0          |

Referenced bits are cleared during the clock interrupt occurs at tick 10.

#### 2. Due to a read request to page 4, page fault occurs at tick 10. Describe the new table entry.

| Page | Time stamp | `Present` | `Referenced` | `Modified` |
|:----:|:----------:|:---------:|:------------:|:----------:|
| 0    | 6          | 1         | 0            | 1          |
| 1    | 9          | 1         | 0            | 0          |
| 2    | 9          | 1         | 0            | 1          |
| 3    | 10         | 1         | 0            | 0          |
| 4    | 4          | 0         | 0            | 0          |

- When page fault occurs at tick 10:
    - Page 0: Scheduled to write
    - Page 1: `Referenced` bit cleared to 0
    - Page 2: `Referenced` bit cleared to 0, then scheduled to write
    - Page 3: $age = 10 - 7 = 3 > τ$ and (`Referenced` == 0 & `Modified` == 0), so evicted and replaced, set time stamp to 10.
    - Page 4: Page loaded into memory.

### Ex. 2 Tracking subprocesses using eBPF
*As a prerequisite, make sure to install BPF Compiler Collection (BCC) toolkit.*

In this exercise, we want to write an eBPF program to track the number of sub-processes forked from it.

#### 1. Which system call should be traced?

> To track the number of sub-processes, the `fork()`, `vfork()`, or `clone()` system calls should be traced. These are the key system calls used to create new processes or sub-processes in Linux.

#### 2. Write an eBPF program based on kprobes and/or kretprobes to implement the task. *Hint*. Quickly search what Kprobes is and how to use it with eBPF.

```C
#include <uapi/linux/ptrace.h>
#include <linux/sched.h>
#include <bcc/proto.h>

BPF_HASH(fork_count, u32, u64);

int on_sys_clone(struct pt_regs *ctx) {
    u32 pid = bpf_get_current_pid_tgid() >> 32;
    u64 *count, zero = 0;

    count = fork_count.lookup_or_init(&pid, &zero);
    (*count)++;
    return 0;
}

int on_sys_clone_ret(struct pt_regs *ctx) {
    return 0;
}

#include <bcc/BPF.h>

int main() {
    BPF bpf;
    bpf.attach_kprobe("do_fork", "on_sys_clone");
    bpf.attach_kretprobe("do_fork", "on_sys_clone_ret");

    printf("Tracking subprocesses...\n");
    bpf.trace_print();
    return 0;
}
```
- Run: `sudo python3 tracker.py`

#### 3. What is `fentry` and how does it differ from `Kprobes`? How could it be used to complete the task?

- `fentry` is a newer type of tracing mechanism that hooks at the very beginning of a function's execution, even before the function’s prologue is complete. It's more efficient than kprobes because it allows attaching at the function entry with lower overhead.
- `kprobes` are more generic and can be placed almost anywhere, but fentry specifically hooks to function entries and is preferred for performance-sensitive scenarios.

### Ex. 3 Research

#### Write about a page on the topic of the `ext2` filesystem.

The **ext2 (Second Extended File System)** is a disk file system designed for the Linux kernel, introduced in January 1993 by French software developer Rémy Card. It was developed to overcome the limitations of its predecessor, the extended file system (ext), and to provide a robust and efficient file system for Linux operating systems. [1]

**ext2** organizes data into blocks, which are grouped into block groups. Each block group contains a copy of critical metadata, such as the superblock and group descriptor table, enhancing date integrity and recovery capabilities. (Structure) [1]

Every file or directory is represented by an inode, which stores metadata including size, permissions, ownership, and disk location. This structure facilitates efficient file management and access. (Inodes) [1]

**ext2** supports individual file sizes up to 2 TiB and file system sizes ranging from 2 TiB to 32 TiB, depending on the block size used. (Maximum Sizes) [1]

Unlike its successor ext3 and ext4, **ext2** does not implement a journaling feature. While this can lead to longer recovery times after system crashes, it reduces the number of write operations, making ext2 suitable for flash-based storage media like USB drives and SD cards, where minimizing write cycles can prolong device lifespan. (Lack of Journaling) [1]


##### Reference
[1] ext2 (https://en.wikipedia.org/wiki/Ext2)

### Ex. 4 Simple Questions
#### 1. If a page is shared between two processes, is it possible that the page is read-only for one process and read-write for the other? Why or why not?

Yes, a shared page can have different permissions for different processes because each process has its own **page table**, which maps virtual addresses to the same physical page with separate permissions. The operating system sets these permissions individually for each process. For example:

- Process A can have the page marked as read-only.
- Process B can have the same page marked as read-write.

This is possible because memory permissions are managed at the process level through their respective page table entries. A common example is **copy-on-write (COW)** during `fork()`, where shared pages are initially read-only and permissions are updated based on access.

#### 2. When both paging and segmentation are being used, first the segment descriptor is found and then the page descriptor. Does the TLB also need a two-levels lookup?

No, the **TLB** does not require a two-level lookup when both segmentation and paging are used. Segmentation translates a logical address into a linear address before paging is applied. The TLB only caches mappings between **linear page numbers** (after segmentation) and **physical page frames** (after paging). It operates after segmentation and does not handle segment descriptor lookups.

## ECE4821 Exercises

### Ex. 5 Simple Question

#### A computer provides each process with 65,536 bytes of address space divided into pages of 4096 bytes. A particular program has a text size of 32,768 bytes, a data size of 16,386 bytes, and a stack size of 15,870 bytes. Will this program fir in the address space? If the page size were 512 bytes, would it fit?

- Page of *4,096 bytes*:
    - Number of pages: 
        - Total: 65,536 / 4,096 = 16 pages
        - Text: 32,768 / 4,096 = 8 pages
        - Data: 16,386 / 4,096 = 5 pages
        - Stack: 15,870 / 4,096 = 4 pages
    - Total: $8 + 5 + 4 = 17 > 16$
    - Answer: **NO**

- Page of *512 bytes*:
    - Number of pages: 
        - Total: 65,536 / 512 = 128 pages
        - Text: 32,768 / 512 = 64 pages
        - Data: 16,386 / 512 = 33 pages
        - Stack: 15,870 / 512 = 31 pages
    - Total: $64 + 33 + 31 = 128$
    - Answer: **YES**

### Ex. 6 Research
#### Write about half a page on the topic of the *minix* file system.

The **MINIX file system** is the native file system of the MINIX operating system, developed by Andrew S. Tanenbaum in the 1980s as a simplified version of the Unix File System. It was designed primarily for educational purposes, offering a clear and concise example of file system implementation. [1]

The file system is composed of several key components: the boot block, which contains the boot loader; the superblock, which stores metadata about the file system such as the number of inodes, zones, and other structural information; the inode bitmap and zone bitmap, which track the usage of inodes and data zones, respectively; the inodes area, which represents each file or directory with metadata like ownership, permissions, timestamps, and pointers to data zones; and the data area, where the actual file and directory content is stored. (Structure) [1]

The MINIX file system had several limitations, such as a maximum partition size of 64 megabytes and a filename length limit of 14 characters (later extended to 30). These constraints made it unsuitable for larger or more complex systems. Nevertheless, its simplicity made it a foundational tool for operating system development. For example, Linus Torvalds initially used the MINIX file system when he started developing the Linux kernel in 1991. However, these limitations eventually led to the development of more advanced file systems like the extended file system (ext) and its successors. (Limitation) [1]

##### Reference
[1] MINIX file system (https://en.wikipedia.org/wiki/MINIX_file_system)

### Ex. 7 Tracking subprocesses using eBPF, again!
The Linux kernel provides trace-points which allow to attach at points other than functions.

#### 1. Looking at `/sys/kernel/tracing/available_events`, which event may be useful to track the number of sub-processes? *Hint*. Restrict your attention to lines with the sched keyword.

> To track the number of sub-processes, we can use the tracepoint event `sched:sched_process_fork`. This event is used to monitor when a new process is forked, making it suitable for tracking sub-process creation.

#### 2. What is should the data structure for the trace-point look like? *Hint*. Search `/sys/kernel/debug/tracing/events/sched/yourselectedtracepoint/format`

```bash
sudo cat /sys/kernel/debug/tracing/events/sched/sched_process_fork/format
```

```C
struct sched_process_fork_args {
    unsigned short common_type;
    unsigned char common_flags;
    unsigned char common_preempt_count;
    int common_pid;

    char parent_comm[16];
    pid_t parent_pid;
    char child_comm[16];
    pid_t child_pid;
};
```

#### 3. Rewrite your program from exercise 2 to use a trace-point instead of a system call.

```C
#include <uapi/linux/ptrace.h>
#include <linux/sched.h>
#include <bcc/proto.h>

BPF_HASH(fork_count, u32, u64);

TRACEPOINT_PROBE(sched, sched_process_fork) {
    u32 parent_pid = args->parent_pid;
    u32 child_pid = args->child_pid;
    u64 *count, zero = 0;

    count = fork_count.lookup_or_init(&parent_pid, &zero);
    (*count)++;
    bpf_trace_printk("Parent PID: %d, Child PID: %d\n", parent_pid, child_pid);
    return 0;
}

#include <bcc/BPF.h>

int main() {
    BPF bpf;
    bpf.attach_tracepoint("sched:sched_process_fork", "sched_process_fork");

    printf("Tracking subprocesses using tracepoints...\n");
    bpf.trace_print();
    return 0;
}
```
- Run: `sudo python3 tracker.py`