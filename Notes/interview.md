# Interview Questions:

## 1. Explain the difference between the *stack* and the *heap*

- *stack*:  

        The stack is used to store the order of method execution and local variables.

- *heap*:

        The heap memory stores the objects and it uses dynamic memeory allocation and deallocation.

---

## 2. What's the difference between user space and kernel space?
The **user space**  and **kernel space**  are two distinct areas of memory in an operating system, used to separate application execution from core system operations.

**User Space**  
- **Definition** : The memory area where user applications (e.g., text editors, browsers) run.
 
- **Access** : Has limited access to system resources to ensure security and stability.
 
- **Purpose** : Provides a safe environment for running programs without directly interacting with hardware.
 
- **Example** : An application requesting a file operation must make a system call to access kernel services.

**Kernel Space**  
- **Definition** : The memory area where the OS kernel and critical system components execute.
 
- **Access** : Has full access to all hardware and system resources.
 
- **Purpose** : Manages hardware, memory, processes, and security.
 
- **Example** : Drivers, interrupt handling, and process scheduling occur in kernel space.

### Key Difference 
 
- **User Space** : Limited privileges to prevent accidental or malicious damage.
 
- **Kernel Space** : Full privileges for controlling and managing hardware and resources.

---

## 3. What are the possible states of a process?

The **possible states of a process**  represent its status during execution. The key states are:  

**1. New** 
- The process is being created and initialized.
- It hasn’t started execution yet.

**2. Ready** 
- The process is loaded into memory and waiting to be assigned to the CPU.
- It is prepared to execute but is in the queue for scheduling.

**3. Running** 
- The process is actively being executed by the CPU.
- Only one process per CPU core can be in this state at a time.

**4. Waiting (or Blocked)** 
- The process is waiting for an event to occur, such as I/O completion or a resource becoming available.
- It cannot proceed until the event happens.

**5. Terminated (or Exit)** 
- The process has completed its execution or has been explicitly terminated.
- It releases all allocated resources.

**State Transitions** 
Processes move between these states based on scheduling, I/O, or completion of tasks. For example:
- **Ready → Running** : Scheduled by the CPU.
- **Running → Waiting** : I/O or resource request.
- **Running → Terminated** : Task completion.

---

## 4. What's an orphan process? What about zombie process?
- **Orphan Process**: Parent terminates before the child; the OS adopts the child process.

- **Zombie Process**: Child terminates but remains in the process table until the parent collects its exit status.

---

## 5. Can processes ignore signals?
Processes can ignore certain signals, but there are exceptions. The ability to ignore signals depends on the signal type and the handling configuration set by the process.

#### Signals a Process Can Ignore  
- Most signals can be ignored by setting their signal handler to `SIG_IGN` using the `signal()` or `sigaction()` system calls.

- Example:

    ```C
    signal(SIGINT, SIG_IGN); // Ignore Ctrl+C (SIGINT)
    ```

- Signals like `SIGINT` (interrupt) and `SIGHUP` (hangup) are often ignored by background processes to prevent unintended interruptions.

#### Signals a Process Cannot Ignore

- `SIGKILL`: This signal forcefully terminates the process and cannot be caught, blocked, or ignored. It is used to ensure a process terminates.

- `SIGSTOP`: This signal pauses a process and also cannot be caught, blocked, or ignored. It ensures the process halts temporarily.

#### When Ignoring Signals is Useful
- Ignoring signals can be useful in scenarios like:
    - Long-running or critical background tasks where interruptions are not desired.
    - Child processes handling signals separately from the parent process.

---
## 6. List some common scheduling algorithms.
### **Common Scheduling Algorithms**

1. **First-Come, First-Served (FCFS)**
   - Processes are scheduled in the order they arrive.
   - **Pros**: Simple to implement.
   - **Cons**: Long waiting times for later processes (convoy effect).

2. **Shortest Job Next (SJN)**
   - The process with the shortest execution time is scheduled first.
   - **Pros**: Minimizes average waiting time.
   - **Cons**: Requires knowledge of process execution times; prone to starvation of longer processes.

3. **Round Robin (RR)**
   - Each process is given a fixed time slice (quantum) in a cyclic order.
   - **Pros**: Fair to all processes.
   - **Cons**: Inefficient for short tasks if quantum is too long.

4. **Priority Scheduling**
   - Processes are scheduled based on priority, with higher-priority processes executed first.
   - **Pros**: Handles important tasks quickly.
   - **Cons**: Risk of starvation for lower-priority processes (can be mitigated with aging).

5. **Multilevel Queue Scheduling**
   - Processes are divided into different queues based on characteristics, with each queue having its own scheduling policy.
   - **Pros**: Separates processes by priority or type.
   - **Cons**: Complex to implement and manage.

6. **Multilevel Feedback Queue**
   - Similar to multilevel queue scheduling, but processes can move between queues based on their behavior.
   - **Pros**: Adaptive; combines advantages of other algorithms.
   - **Cons**: Requires fine-tuning of parameters.

7. **Shortest Remaining Time (SRT)**
   - Preemptive version of SJN; the process with the shortest remaining time is executed.
   - **Pros**: Optimizes turnaround time.
   - **Cons**: Starvation of longer tasks.

8. **Earliest Deadline First (EDF)**
   - Used in real-time systems; schedules processes with the earliest deadlines first.
   - **Pros**: Ideal for deadline-critical tasks.
   - **Cons**: Requires precise timing and can overload the CPU.

---
## 7. Talk about context change.

### **Context Change (Context Switching)**

**Context change** occurs when the CPU switches from executing one process (or thread) to another. This is essential for multitasking and scheduling but incurs an overhead.

#### **Steps in Context Switching**
1. **Save the Current State**:
   - The CPU state (registers, program counter, etc.) of the running process is saved to its process control block (PCB).
2. **Load the Next Process**:
   - The state of the new process to be executed is loaded from its PCB.
3. **Resume Execution**:
   - The CPU resumes the new process from where it was last paused.

#### **Cost of Context Switching**
- **CPU Overhead**: Switching takes time during which the CPU does no productive work.
- **Cache Effects**: The CPU cache may need to be reloaded for the new process, impacting performance.
- **Minimization**: Efficient scheduling algorithms and reducing the frequency of switches help minimize overhead.

#### **Key Considerations**
- Context switching is critical for multitasking but should be optimized to balance responsiveness and system performance.


What's the difference between user space and kernel space?
What's the difference between stacks and heaps?
What are the possible states of a process?
What's an orphan process? What about zombie process?
Can processes ignore signals?
List some common scheduling algorithms.
Talk about context change.
What are the primary means of inter-process communication (IPC)?
What are the differences between processes, threads and coroutines?
What are the prerequisites for a deadlock to happen?
How to prevent deadlocks?