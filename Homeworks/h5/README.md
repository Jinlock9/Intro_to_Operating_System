# h5

## ECE4820 Exercises

### Ex. 1 Simple Questions

1. A system has two processes and three identical resources. Each process needs a maximum of two resources. Can a deadlock occur? Explain.

    Since there are three identical resources and each process needs a maximum of two, there will always be enough resources to fulfill at least one process's maximum need. If both processes hold one resource each and one additional resource remains, either process can request and obtain the second resource it needs. Because we cannot reach a situation where both processes are indefinitely waiting for each other to release resources, the **circular wait** cannot be satisfied. Therefore, deadlock is impossible.

2. A computer has six tape drives, with *n* processes competing for them. Each process may need two drives. For which values of *n* is the system deadlock free?

    For deadlock to be avoided, we can follow the **Deadlock Prevention Condition**:

    
    $\text{Total resources R} \geq (\text{Max need per process d} - 1) \times \text{Number of processes n} + 1$

    * R = 6
    * d = 2

    * $6 \geq (2 - 1) \times \text{n} + 1$
    * $n \leq 5$

    If there are **5 or fewer** processes, the system will not enter deadlock.

3. A real-time system has four periodic events with periods of 50, 100, 200, and 250 msec each. Suppose the four events require 35, 20, 10, and x msec of CPU time, respectively. What is the largest value x for which the system is schedulable.

    $35/50 + 20/100 + 10/200 + x/250 < 1$  
    $x < 12.5$  

    **x** should be **less than 12.5 msec**.

4. Round-robin schedulars normally maintain a list of all runnable processes, with each process occurring exactly once in the list. What would happen if a process occurred more than once in the list? Would there be any reason for allowing this?

    If a process appears more than once in a round-robin schedular's list, it receives extra CPU time compared to others (it is granted with more than one turn to run), increasing the process's priority. 

5. Can a measure of whether a process is likely to be CPU bound or I/O bound be detected by analyzing the source code. How to determine it at runtime?

    **Yes**. If there are a number of calculations, it will be CPU bounded. Howerver, if there are more reading or writing operations, it will be I/O bounded.

### Ex. 2 Deadlocks
Assuming three resources consider the following snapshot of a system.

| Process | Allocated | Maximum | Available |
|:-------:|:---------:|:-------:|:---------:|
|  *P1*   |   0 1 0   |  7 5 3  |    332    |
|  *P2*   |   2 0 0   |  3 2 2  |           |
|  *P3*   |   3 0 2   |  9 0 2  |           |
|  *P4*   |   2 1 1   |  2 2 2  |           |
|  *P5*   |   0 0 2   |  4 3 3  |           |

1. Determine the content of the Request matrix.

    $\text{Request} = \text{Maximum} - \text{Allocated}$

    - Request Matrix:

        7   4   3  
        1   2   2  
        6   0   0  
        0   1   1  
        4   3   1

2. Is the system in a safe state?

    **Yes**. Since all processes can complete in the order (ex. P2, P4, P5, P1, P3), the system is in a **safe state**.

3. Can all the processes be completed without the system being in an unsafe state at any stage?

    **Yes**, all processes can be completed in the sequence (ex) P2, P4, P5, P1, P3 without the system being in an unsafe state at any stage.

    - Available: **( 3 3 2 )**
    - Execute **P2** -> Available: **( 2 1 0 )**
    - Available: **( 5 3 2 )**
    - Execute **P4** -> Available: **( 5 2 1 )**
    - Available: **( 7 4 3 )**
    - Execute **P5** -> Available: **( 3 1 2 )**
    - Available: **( 7 4 5 )**
    - Execute **P1** -> Available: **( 0 0 2 )**
    - Available: **( 7 5 5 )**
    - Execute **P3** -> Available: **( 1 5 5 )**
    - Available: **( A 5 7 )**

### Ex. 3 The reader-writer problem
In the *reader-writer problem*, some data could be accessed for reading but also sometimes for writing. When processes want to read the data they get a *read lock* and a *write lock* fo writing. Multiple processes could get a read lock at the same time while a write lock should prevent anybody else from reading or writing the data until the write lock is released.

To solve the problem we decide to use a global variable `count` together with two semaphores: `count_lock` for locking the `count` variable, and `db_lock` for locking the database. To get a write lock, we can proceed as follows:

```C
void write_lock() {
    down(db_lock);
}
```

```C
void write_unlock() {
    up(db_lock);
}
```

1. Explain how to get a read lock, and write the corresponding pseudocode.

    - To get a read lock, when a reader wants to access the data, it increments the `count` variable. It it is a first reader, then acquire the `db_lock` to prevent any writers to access.
    - Subsequent readers also increment `count` but do not need to acquire `db_lock`.
    - When a reader is done, decrements `count`.
    - If `count` becomes 0, release `db_lock`.

    ```C
    size_t count = 0;
    semaphore count_lock;
    semaphore db_lock;

    void read_lock() {
        down(count_lock);
        count++;
        if (count == 1) {
            down(db_lock);
        }
        up(count_lock);
    }

    void read_unlock() {
        down(count_lock);
        count--;
        if (count == 0) {
            up(db_lock);
        }
        up(count_lock);
    }
    ```

2. Describe what is happening if many readers request a lock.

    While many readers request a read lock, the first reader lockas `db_lock`, blocking writers, while subsequent readers increment `count` without waiting, allowing all readers to access the data concurrently. This is efficient for concurrent readers, but writers may face starvation if readers continuously request access, as `db_lock` is only released once all readers are done.

To overcome the previous problem we will block any new reader when a write becomes available.

3. Explain how to implement this idea using another semaphore called `read_lock`.

    Now, `read_lock` semaphore will be used to block any new readers if a writer is waiting for access. When a writer wants to access the data, it will acquire the `read_lock`. Once a writer has completed, it releases  `read_lock`, allowing readers to resume.

    ```C
    size_t count = 0;
    semaphore count_lock;
    semaphore read_lock;
    semaphore db_lock;

    void write_lock() {
        down(read_lock);
        down(db_lock);
    }

    void write_unlock() {
        up(db_lock);
        up(read_lock);
    }

    void read_lock() {
        down(read_lock);
        down(count_lock);
        count++;
        if (count == 1) {
            down(db_lock);
        }
        up(count_lock);
        up(read_lock); 
    }

    void read_unlock() {
        down(count_lock);
        count--;
        if (count == 0) {
            up(db_lock);
        }
        up(count_lock);
    }
    ```

4. Is this solution giving any unfair priority to the writer or the reader? Can the problem be considered as solved?

    - This solution introduces an unfair priority to **writers**.
    
    - **Not completely**, because this solution prevents writer starvation but introduces reader starvation under high writer demand.

## ECE4821 Exercise

### Ex. 4 Programming
Implement the Banker's algorithm.

- banker.cpp
- build: `g++ -std=c++17 -Wall -Wextra -o banker banker.cpp`