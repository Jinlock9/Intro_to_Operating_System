# Interprocess Communication

## Exhibiting the problem
### Working with a single thread
In single tasking, all the threads are independant:
- They cannot affect of be affected by anything
- Their state is not shared with other threads
- The input state determines the output
- Everything is reproducible
    - In the context of single-threaded environment, *reproducible* means that if you start with the same input state and run the program under the same condition, you will always get the same output.
- Stopping or resuming does not lead to any side effect

### Working with multiple threads
Difficulties apprear with multi-tasking:
- A thread runs on one core at a time
- A thread can run on different cores at different times
- Each core is shared among several threads
- Several cores run several threads in parallel
- The number of cores has no impact on the running of the threads

*Changes made by one thread can affect others*

### Thread and process cooperation
Setup for threads:
- Several threads share a common global variable
- The execution sequence impacts the global variable
- By default, the behavior is random and irreproducible  

Major problems:
- How can threads share information?
- How to prevent them from getting on each other's way?
- How to ensure an acceptable running order?

*All those thread issues within a process can be extened to processes withing the operating system*

### Race Condition
A **race condition** occurs in a multi-threaded environment when two or more threads access shared resources (like variable or memory) concurrently, and the final outcome depends on the timing or order of execution of those threads. This can lead unpredictable, incosistent, or erroneous behavior.  

**Key Characteristics**
- Concurrent Access
- At least One Write
- Timing-Dependent


## Solving the problem
### Critical region
**Critical Region**: Part of the program where shared memory is accessed
- No two processes can be in a critical region at a same time
- No assumption on the speed or number of CPUs
    - solution for managing access to the critical region must work correctly regardless of the speed or number of CPUs
- No process outside a critical region can block other processes
- No process waits forever to enter a critical region

### Too much milk [1]
1. Frank & John both checking `no milk && no note`
    ```C
    if (no milk && no note) {
        leave note;
        milk the cow;
        remove note;
    }
    ```
    - Problems:
        1) Simultaneous Check:
            - Frank and John might both execute the `if (no milk && no note)` condition at the same time.
            - Both see `no milk` and `no note` as `true` simultaenously.
        2) Duplicate Work:
            - Both leave their own note, proceed to milk the cow, and the remove their note.
    - Why this happnes:
        - There is no mechanism to prevent **both threads** (Frank and John) from simultaneously entering the critical region (`if (no milk && no note)`).
        - **No mutual exclusion**: Both can check the condition at the same time, which leads to incorrect behavior.

2. Leaving and checking notes explicitly
    - Frank

        ```C
        leave note Frank;
        if (no note John) {
            if (no milk) milk the cow;
        }
        remove note Frank;
        ```

    - John

        ```C
        leave note John
        if (no note Frank) {
            if (no milk) milk the cow;
        }
        remove note John
        ```

    - Problems:
        1) Circular Waiting:
            - If Frank leaves a note and John leaves a note at the same time, both will see the other's note and will be stuck waiting indifinitely:
                - Frank sees John's note and waits
                - John sees Frank;s note and waits
            - Neither proceeds to milk the cow, causing a **deadlock**
        2) Timing Dependency
            - If one thread checks for the other thread's note after the notes has been removed but before the milk is added, it might incorrectly assume it can proceed to milk the cow, leading to duplicate or missed milking

### Too much milk [2]
- Frank

    ```C
    leave note Frank;
    while (note John) nothing;
    if (no milk) milk the cow;
    remove note Frank;
    ```

- John

    ```C
    leave note John;
    if (no note Frank) {
        if (no milk) milk the cow;
    }
    remove note John;
    ```

This strategy ensures **correctness** (only one person milks the cow at a time) but suffers from **inefficiency** and **fairness issues**:
- **Busy Waiting**: Frank wastes CPU cycles while waiting for John's note to disappear.
- **Fairness concerns**: Frank might starve if John frequently leaves and remove his note quickly.
- Asymmetry: Frank's busy waiting is inefficient compared to John's simpler check.

### Peterson's Idea
Symmetric strategy for two processes:
- When wanting to enter a critical region a process:
    - Shows its interest for the critical region
    - If it is accessible, it exits the function and accesses it
    - If it is not accessible, it waits in a tight loop
- When a process has completed its work in the critical region, it signals its departure

Drawbacks:

        While Peterson's algorithm is conceptually elegant and guarantees mutual exclusion, its reliance on busy waiting, inefficiency on modern hardware, and lack of scalability limit its practical application. More efficient mechanisms like semaphores, mutexes, or hardware-based atomic operations are preferred in real-world systems.

- **Peterson's Idea**  

    **Steps**:
    1. Indicate intent to enter (`flag[i] = true`):  
        Each process signals that it wants to enter the critical section.
    2. Yield priority to the other process (`turn = j`):  
        Each processes gives the other process a chance to enter the critical section first.
    3. Wait for the other process to finish:  
        A process can only proceed to the critical section if:
        - The other process doesn't want to enter (`flag[j] == false`) or
        - It's the current process's turn (`turn == i`).
    4. Critical section execution:  
        Once allowed, the process enters its critical section.
    5. Release (`flag[i] = false`):  
        After exiting the critical section, the process signals that it no longer wants access.

    ```C
    flag[i] = true;  // Indicate intent to enter
    turn = j;        // Yield priority to process j
    while (flag[j] && turn == j); // Wait until process j finishes
    // Critical Section
    flag[i] = false; // Indicate leaving the critical section
    ```

    **Key Properties**
    - **Mutual Exclusion**: Only one process can enter the critical region at a time.
    - **Progress**: If no process is in the critical region, one of the waiting processes will eventually enter.
    - **Bounded Waiting**: Each processes gets a fair turn and doesn't wait forever.

#### Pseudo Code
```C
 1 #define TRUE 1
 2 #define FALSE 0
 3 int turn;
 4 int interested[2];
 5 void enter_region(int p){
 6      int other;
 7      other = 1-p;
 8      interested[p]=TRUE;
 9      turn = p;
 10     while (turn == p && interested[other] == TRUE);
 11 }
 12 void leave_region(int p){
 13     interested[p] = FALSE;
 14 }
```

#### Side effects of Peterson's Idea:
- Two processes: L, low priority, and H, high priority
- L enters in a critical region
- H becomes ready
- H has higher priority so the scheduler switches to H
- L has lower priority so is not rescheduled as long a H is busy
- H loops forever

### Mutual exclusion at hardware level
#### Prevent the process in the critical region from being stopped:
Two approaches to ensure that a process in the critical region cannot be interrupted of interfered with, thereby protecting shared resources.  

1. Disable interrupts:  
    **Disabling Interrupts** temporarily prevents the CPU from switching to another process by stopping the handling of interrupt signals.  

    - Can be done within the kernel for a few instructions
    - Cannot be done by user processes
    - Only works when there is a single CPU
    - An interrupt on another CPU can still mess up the shared variable.

2. Use atomic operations:  
    An **Atomic operations** is a low-level, indivisible operations that either completes entirely or does not happen at all (no partial update).  

    - Either happens in its entirety or not at all
    - Several operations can be performed at once, e.g., *A = B*
    - Requires the CPU to support the atomic update of a memory space
    - Can be used to prevent other CPUs to access a shared memory
    - Ensure that a critical operation is executed as an indivisible unit.

### Using the *TSL* instruction
A simple atomic operation:
- Test and Set Lock: `TSL`
- Copies `LOCK` to a register and set it to `1`
- `LOCK` is used to coordinate the access to a shared memory
- Ensure `LOCK` remains unchanged while checking its value

```asm
enter_region:
    TSL REGISTER, LOCK ; Atomically copy LOCK to REGISTER and set LOCK to 1
    CMP REGISTER, #0   ; Compare the old value of LOCK (in REGISTER) with 0
    JNE enter_region   ; If LOCK was not 0 (already locked), retry
    RET                ; If LOCK was 0, successfully entered critical region

critical_region:

leave_region:
    MOVE LOCK, #0      ; Set LOCK to 0 to indicate the resource is free
    RET                ; Return
```

#### What is `TSL`?
1. **TSL (Test and Set Lock)** is an atomic instruction that:
    - **Reads** the current value of a memory location (e.g., `LOCK`) into a register.
    - **Sets** the memory location (e.g., `LOCK`) to `1` in a single, uninterruptible step.
2. The atomic nature of `TSL` ensure that no other process can access or modify the value of `LOCK` during this operation.
3. **Purpose**: `LOCK` is used as a flag to control access to a shared resource:
    - `LOCK = 0`: The resource is free (unlocked).
    - `LOCK = 1`: The resource is busy (locked).

### Peterson's Algorithm vs TSL
- **When is TSL Better?**
    - In modern systems with hardware support for atomic instructions.
    - For short critical sections where busy waiting does not cause significant performance issues.
    - In multi-core or multi-CPU environments, where Peterson’s algorithm becomes impractical.
- **When is Peterson’s Algorithm Better?**
    - In environments where hardware atomic instructions (like TSL) are unavailable.
    - For systems with only **two processes**, where its fairness and lack of hardware dependency make it a simple and effective solution.
    - When minimizing **busy waiting** is a priority (e.g., in systems with constrained resources or long critical sections).

### Consumer-producer problem
```C
1  #define N 100
2  int count = 0;
3  void producer() {
4      int item;
5      while(1) {
6          item=produce_item(); if(count==N) sleep();
7          insert_item(item); count++;
8          if(count==1) wakeup(consumer);
9      }
10 }
11 void consumer() {
12     int item;
13     while(1) {
14         if(count==0) sleep();
15         item=remove_item(); count--;
16         if(count==N-1)wakeup(producer); consume_item(item);
17     }
18 }
```
#### Potential Problems:
1. **Race Conditions on `count`**:

    - If the producer and consumer access `count` simultaneously, a **race condition** can occur, leading to inconsistent or incorrect values.

2. **Lost Wakeups**:

    - If a producer or consumer calls `wake up()` before the other thread has gone to sleep, the wake-up signal is **lost**. This leaves the sleeping thread in a blocked state indefinitely, causing a **deadlock**.

3. **No Synchronization on the Shared Buffer**:

    - The `insert_item()` and `remove_item()` functions likely manipulate a shared buffer, but there is no synchronization to ensure mutual exclusion. This can lead to data corruption or undefined behavior when both threads access the buffer simultaneously.

#### Assume the buffer is empty:
- Comsumer reads `count == 0`
- Schduler stops the consumer and starts the producer
- Producer adds one item
- Producer wakes up the consumer
- Consumer not yet asleep, *signal is lost*
- Consumer goes asleep
- When the buffer is full, the producer falls asleep
- Both consumer and producer sleep forever

### Semaphore
A semaphore `sem` is:
- A positive interger variable
- Only changed or tested through two actions
    ```C
    sem.down() {
        while (sem == 0) sleep(); 
        sem--;
    }

    sem.up() {
        sem++;
    }
    ```
    - The `down` operation:
        - If `sem > 0`, decrease it and continue
        - If `sem = 0`, blocked and do not complete the down

    - The `up` operation:
        - Increment the value of the semaphore
        - An awaken sleeping process can complete its down

- Semaphores maintain a counter to represent the availability of resources:
    - **Positive Value**: The number of resources available.
    - **Zero or Negative Value**: Threads/processes are blocked, waiting for resources to be released.

### Semaphores and processes

- Checking or changing the value and sleeping are done **atomically**:
    - Single CPU: disable interrupts
    - Multiple CPUs: use *TSL* to ensure only one CPU accesses the semaphore

### Semaphores and interrupts
Using semaphores to hide interrupts:
- Each IO device gets a semaphore initialized to 0
- A process accessing the device applies a `down`
- The process becomes blocked
- An interrupt is issued when the device has completed the work
- The interrupt handler processes the interrupt and applies an `up`
- The process becomes ready

### Mutex
A **mutex** is a semaphore taking values `0 (unlocked)` or `1 (locked)`.  

#### On a mutex-lock request:
- If the mutex is unlocked:
    - Lock the mutex
    - Enter the critical region
- If mutex is locked: put the calling thread asleep
- When the thread in the critical region exits:
    - Unlock the mutex
    - Allow a thread to acquire the lock and enter the critical region

### Mutex Implementation
Mutexes can be implemented in user-space using *TSL*.

```asm
mutex-lock:
   TSL REGISTER,MUTEX
   CMP REGISTER,#0
   JZ ok
   CALL thread_yield
   JMP mutex-lock
ok: RET
 
mutex-unlock:
   MOVE MUTEX,#0
   RET
```

#### Explain how `mutex_lock` differs from `enter_region`?

- `mutex_lock` reduces CPU wastage by using `thread_yield` to voluntarily give up the CPU when the lock is unavailable.
- `enter_region` relies on pure *busy-waiting*, consuming CPU cycles unnecessarily in a tight loop.

#### In user-space what happens when a thread tries to acquire a lock through busy-waiting?

- **Busy-waiting** wastes CPU cycles and can lead to poor performance in multi-threaded applications.
- It can starve other threads or processes of CPU time, especially in a user-space environment where threads lack direct control over the CPU scheduler.

#### Why use `thread_yield` used?
- **thread_yield** allows the current thread to voluntarily give up the CPU and let other threads run.
- *Improves Efficiency*: Reduces CPU usage by allowing other threads to execute.
- *Ensures Fairness*: Gives other threads a chance to progress, reducing thread starvation.
- *Better Utilization*: Helps avoid unnecessary spinning and allows threads to be rescheduled when the lock becomes available.