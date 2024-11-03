# [ECE482] P1 Presentation Topic 8: Background Processes

518370990004 Park Jae Heung

520370990012 Jinlock Choi

## Introduction

Up until now, process execution assumed foreground tasks

     - i.e. the shell fully waits until the command finishes executing

**Background tasks** are when the shell continues executing while the command executes.

**Syntax**: the difference is the inclusion of the '&' at the end of the command

    - `ls` : foreground

    - `ls &` : background

## Waiting

We used `waitpid()` to block the shell until the foreground process finishes.

However, we cannot use wait the same way for background processes!

    - But, we still need to wait to capture when the command finishes.

    - Otherwise, our child process will become a zombie.

## WNOHANG (`waitpid()` in Non-blocking mode)

`waitpid(pid, &status, WNOHANG)`: Return pid of changed process, 0 if still running.

This causes `waitpid` to return immediately.

But it captures whether the process (pid) has finished or not

    - It only tells  us when we check

    - This means we will have to check repeatedly.

## Simple Implementation (Example)

```c
pid_t pid = fork();
if (pid < 0) {
    // ERROR!
}
else if (pid == 0) {
    // Child process : EXECUTE COMMAND ~
}
else {
    // Parent process
    if (!background) { // FOREGROUND PROCESS!
        waitpid(pid, NULL, 0);
    }
    else { // BACKGROUND PROCESS
        // NO WAIT!
    }
}
```

Also, we need to reap any finished background processes periodically.

```c
while (waitpid(-1, NULL, WNOHANG) > 0) {
    // Reap background processes
}
```

## Exit

What happens if we have background processes and exit is issued?

    - We will need to fully wait on each of them before quiting.

    - This requires keeping a counter or array og executing processes.


