# h3

## Exercises
### Ex. 1 General questions

1.  If a multithreaded process forks, a problem occurs if the child gets 
copies of all the parent's threads. Suppose that one of the original 
threads was waiting for keyboard input. Now two threads are waiting for 
keyboard input, one in each process. Does this problem ever occur in 
single-threaded processes?

    > No, the problem does not occur in single-threaded processes because in a single-threade process, only one thread exists. When a single-threaded process forks, the child process gets a copy of that single thread. Since there is only one thread, there is no possibility of multiple threads waiting for the same resource, such as kerboard input. The child process will independently handle its own I/O, and the parent continues with its own, without the complications introduced by multiple threads managing the same resource.

2. Many UNIX system calls have no Win32 API equivalents. For each such 
call, what are the consequences when porting a program from a UNIX 
system to a Windows system?

    > The main consequence of the absence of equivalent Win32 API calls is that the original program must be restructed or rewritten ot use Windows-specific APIs, often requiring significant code changes to adapt to the different system models of process management, file handling, security, and IPC.

### Ex. 2 C programming

- Source files:
    - ex2/ex2.c
    - ex2/ex2.h 

- Compile: `make`

### Ex. 3 Research on POSIX
Write a few paragraphs about the POSIX standards. What are they, why do 
they exist, what kind of things are included in the norms.

    POSIX stands for "Portable Operating System Interface." It's a set of rules or standards made so that different computer operating systems, especailly those similar to UNIX, can work in a similar way.
    In the past, different versions of UNIX systems had their own ways of doing things, which made it tricky for software developers. If they made a program for one system, it might not work on another. The POSIX standards were introduced to fix this, making it easier for software to run on multiple systems without lots of changes.
    POSIX covers how software programs should talk to the system (Software Interactions), how the command line (where you type commands) should work (Command Lines), basic tools and programs that most systems should have (Utilities), and rules about managing multiple things happening at once, like opening multiple apps (Handling Tasks).

### Ex. 4 Thinking of threads

1. Why would a thread voluntarily release the CPU?

    > A thread may voluntarily release the CPU when it is waiting for I/O, resources, or a specific event to avoid waisting CPU cycles. This helps improve system efficieny and allows other threads to execute, enhancing multitasking and resource utilization.

2. What is the biggest advantage/disadvantage of user space threads?

    > The biggest advantage of user space threads is that they are lightweight and have minimal overhead since thread management (creationn, scheduling, and context switching) is handled by the user-space library without needing kernel intervention, making them faster. The biggest disadvantage is that they do not take full advantage of multi-core processors because the kernel is unaware of these threads and cannot schedule them independently on multiple cores, leading to poor performance in CPU-bound tasks.