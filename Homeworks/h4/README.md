# h4
## ECE4820 Exercises
### Ex 1. Simple Questions
1. Consider a system in which threads are implemented entirely in user space, with the run-time system getting a clock interrupt once a second. Suppose that a clock interrupt occurs while some thread is executing in the run-time system. What problem might occur? Can you suggest a way to solve it?

    > Because the run-time system might be in the middle of the operations like scheduling a thread, updating thread states, modifying thread control block, etc, there is a potential for the system to get into an inconsistent state if a clock interrupt occurs while some thread is executing in the run-time system.  
    The solution is to disable clock interrupts when the runtime system is executing critical section of code (e.g., thread scheduling, state updating, etc.).

2. Suppose that an operating system does not have anything like the `select` system call to see in advance if it is safe to read from a file, pipe, or device, but it does allow alarm clocks to be set that interrupt blocked system calls. Is it possible to implement a threads package in user space under these conditions? Discuss.

    > Yes, the role of `select` is to ensure whether it is safer for I/O operation, and this can be replaced by alarm clock interrupts. To do I/O operations in a thread, all other threads will be interrupted by alarm clock.

### Ex. 2 Race condition in Bash
Write a bash script which generates a file composed of one integer per line. The script should read the last number in the file, add one to it, and append the result to the file.

- Sources:
    - ex2/
        - ex2.sh
        - driver.sh
        - ex2_modified.sh

1. Run the scipt in both background and foreground at the same time. How long does it take before observing a race condition?

    - ex2.sh

        ```sh
        #!/bin/bash

        FILE=./ex2.out
        if ! [ -f $FILE ]; then
            echo 0 >> $FILE
        fi

        for i in {1..10}
        do
            op=$(tail -n 1 $FILE)
            ((op++))
            echo $op >> $FILE
        done
        ```

    - driver.sh

        ```sh
        #!/bin/bash

        for i in {1..10}
        do
            ./ex2.sh; ./ex2.sh&
        done
        ```

    > The **race condition** happens after the number **10**.

2. Modify the script such as to prevent the race condition.

    - ex2_modified.sh

        ```sh
        #!/bin/bash

        FILE=./ex2_modified.out
        if ! [ -f "$FILE" ]; then
            echo 0 >> $FILE
        fi

        for i in {1..10}
        do (
            flock -n -x 99 || exit 1
            op=$(tail -n 1 $FILE)
            ((op++))
            echo $op >> $FILE
        ) 99>>$FILE
        done
        ```

### Ex. 3 Programming with semaphores

1. On Linux, find the file *semaphore.h*.

    - path:
        - /usr/include/semaphore.h

2. Read the documentation to understand how to use the functions described in the file *semaphore.h*.

    ```C
    /* Copyright (C) 2002-2024 Free Software Foundation, Inc.
    This file is part of the GNU C Library.

    The GNU C Library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    The GNU C Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with the GNU C Library; if not, see
    <https://www.gnu.org/licenses/>.  */

    #ifndef _SEMAPHORE_H
    #define _SEMAPHORE_H    1

    #include <features.h>
    #include <sys/types.h>
    #ifdef __USE_XOPEN2K
    # include <bits/types/struct_timespec.h>
    #endif

    /* Get the definition for sem_t.  */
    #include <bits/semaphore.h>


    __BEGIN_DECLS

    /* Initialize semaphore object SEM to VALUE.  If PSHARED then share it
    with other processes.  */
    extern int sem_init (sem_t *__sem, int __pshared, unsigned int __value)
    __THROW __nonnull ((1));

    /* Free resources associated with semaphore object SEM.  */
    extern int sem_destroy (sem_t *__sem) __THROW __nonnull ((1));

    /* Open a named semaphore NAME with open flags OFLAG.  */
    extern sem_t *sem_open (const char *__name, int __oflag, ...)
    __THROW __nonnull ((1));

    /* Close descriptor for named semaphore SEM.  */
    extern int sem_close (sem_t *__sem) __THROW __nonnull ((1));

    /* Remove named semaphore NAME.  */
    extern int sem_unlink (const char *__name) __THROW __nonnull ((1));

    /* Wait for SEM being posted.

    This function is a cancellation point and therefore not marked with
    __THROW.  */
    extern int sem_wait (sem_t *__sem) __nonnull ((1));

    #ifdef __USE_XOPEN2K
    /* Similar to `sem_wait' but wait only until ABSTIME.

    This function is a cancellation point and therefore not marked with
    __THROW.  */
    # ifndef __USE_TIME_BITS64
    extern int sem_timedwait (sem_t *__restrict __sem,
                            const struct timespec *__restrict __abstime)
    __nonnull ((1, 2));
    # else
    #  ifdef __REDIRECT
    extern int __REDIRECT (sem_timedwait,
                        (sem_t *__restrict __sem,
                            const struct timespec *__restrict __abstime),
                            __sem_timedwait64)
    __nonnull ((1, 2));
    #  else
    #   define sem_timedwait __sem_timedwait64
    #  endif
    # endif
    #endif

    #ifdef __USE_GNU
    # ifndef __USE_TIME_BITS64
    extern int sem_clockwait (sem_t *__restrict __sem,
                            clockid_t clock,
                            const struct timespec *__restrict __abstime)
    __nonnull ((1, 3));
    # else
    #  ifdef __REDIRECT
    extern int __REDIRECT (sem_clockwait,
                        (sem_t *__restrict __sem,
                            clockid_t clock,
                            const struct timespec *__restrict __abstime),
                            __sem_clockwait64)
    __nonnull ((1, 3));
    #  else
    #   define sem_clockwait __sem_clockwait64
    #  endif
    # endif
    #endif

    /* Test whether SEM is posted.  */
    extern int sem_trywait (sem_t *__sem) __THROWNL __nonnull ((1));

    /* Post SEM.  */
    extern int sem_post (sem_t *__sem) __THROWNL __nonnull ((1));

    /* Get current value of SEM and store it in *SVAL.  */
    extern int sem_getvalue (sem_t *__restrict __sem, int *__restrict __sval)
    __THROW __nonnull ((1, 2));


    __END_DECLS

    #endif  /* semaphore.h */
    ```

3. Using semaphores, adjust the program such as to always return the correct answer.

    - Compile: `gcc -o main cthread.c -lpthread`
    
    - Source: h4/cthread.c

## ECE4821 Exercise
### Ex. 4 Monitors
During the lecture, moitors were introduced (3.29|3.105). They use condition variables as well as two instructions, `wait` and `signal`. A different approach would be to have only one operation called `waituntil`, which would check the value of a boolean expression and only allow a process to run when it evaluates as `True`. What would be the drawback of such a solution?

> If `waituntil` is implemented by repeatedly checking the condition in a loop, this could result in **busy-waiting**, where a process continually consumes CPU cycles while waiting for the condition to become `True`.  
Also, even if the `waituntil` implementation avoids busy-waiting by using some form of sleep, **spurious wake-ups** (where a process is woken up without the condition being `True`) may occur.  
