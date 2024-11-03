# ECE482 2024FA H1
## Author
- Name: **JINLOCK CHOI**
- Student No.: **520370990012**
- E-mail: **jinlock99@sjtu.edu.cn**

## Exercises
### Ex. 1 Review
**Explain the difference between the *stack* and the *heap***
- *stack*:
    > The *stack* is used to store the oreder of method execution and local variables.
- *heap*:
    > The *heap* memory stores the objects and it uses dynamic memory allocation and deallocation.

### Ex. 2 Personal research
1. **Briefly explain what operations are performed when a computer is powered on. What is the role of the BIOS and how does it interact with the OS?**
    > When a computer is powered on, the system undergoes a series of operations starting with the Power-On Self-Test (POST), initiated by the BIOS (Basic Input/Output System), which checks if critical hardware components like the CPU, RAM, and storage devices are funcitoning properly. After the POST, the BIOS searches for a bootable device based on the boot order and loads the bootloader from the selected storage device. Once loaded, the bootloader initializes the operating system (OS) by loading its kernel into memory.

    > The BIOS plays a crucial role during this process by initializing hardware and acting as an intermediary between the hardware and software before the OS fully takes control. Once the OS is running, it uses its own drivers to interact with hardware directly, though the BIOS may still manage tasks like power management and system configuration.

2. **In a few words explain what are hybrid and exo kernels.**
    > **Hybrid kernels** combine elements of both monolithic and microkernels, aiming to balance performance and modularity. They run most services, like device drivers and system calls, in kernel space but can move some components to user space for better flexibility and stability.

    > **Exokernels** are minimalist kernels that provide direct access to hardware resources, leaving resource management to applications. This design maximizes efficiency and customizability, as the OS doesn't impose abstractions, but it requires more responsibility from applications for managing hardware.

### Ex. 3 Course application
1. **Which of the following instructions should only be allowed in kernel mode? Explain.**
- Answer: **a, c, d**  

    > **(a) Disable all interrupts**: Disabling interrupts can prevent the system from responding to critical events (e.g., hardware signals or system errors), which could lead to crashes or instability. **Kernel mode**.

    > **(b) Read the time-of-day clock**: Reading the system clock does not pose any security or stability risks, so it can be allowed in user mode. **User mode**.

    > **(c) Set the time-of-day clock**: Setting the clock could affect time-sensitive operations or logs, potentially causing errors or security issues. **Kernel mode**.

    > **(d) Change the memory map**: Modifying the memory map directly alters how memory is allocated and accessed, which could corrupt system memory, leading to crashes or security breaches. **Kernel mode**. 

2. **Consider a system that has two CPUs and each CPU is composed of two threads. Suppose thress programs, P0, P1, and P2, are started with run times of 5, 10, and 20 ms, respectively. How long will it take to complete the execution of these programs? Assume that all three programs are 100% CPU bound, do not block during execution, and do not change CPUs once assigned.**
- If P0 and P1 are assigned to CPU1 and P2 is assigned to CPU2, then 5 + 10 < 20, total time is 20 ms.
- If P0 and P2 are assigned to CPU1 and P1 is assigned to CPU2, then 5 + 20 > 10, total time is 25 ms.
- If P1 and P2 are assigned to CPU1 and P0 is assigned to CPU2, then 10 + 20 > 5, total time is 30 ms.
- If all programs are assigned to single CPU, then 5 + 10 + 20 = 35 ms.

    > Therefore, **20 ms, 25 ms, 30 ms, and 35 ms.**

### Ex. 4 Command lines on a Unix system
**On linux use the command line to:**  
1. **Create a new user:**  
    `user add -m jinlock`
2. **List all the currently running processes:**  
    `ps -ael`
3. **Display the characteristics of the CPU and the available memory:**  
    - `cat /proc/cpuinfo`
    - `cat /proc/meminfo`
4. **Redirect random output read from a system device, into two different files:**  
    `head -n 1 /dev/random | tee 1.txt > 2.txt`
5. **Concatenate the two previous files:**  
    `cat 1.txt 2.txt > 3.txt`
6. **Read the content of the resulting file as hexadecimal values (in other words find a command to read a file as hexadecimal values):**  
    `hexdump 3.txt`
7. **Go at the root of the kernel source you downloaded in l2 and use a single command to find all the files in with the word `mutex` in their name and containing the whole word `nest_lock`:**  
    `find . -name "*mutex*" | xargs grep -rlw "nest_lock"`

### Ex. 5 Simple problem
**One reason GUIs were initially slow to be adopted was the cost of the hardware needed to support them. How much video RAM is needed to support a 25 lines by 80 rows character monochrome text screen? How much for a 1024 x 768 pixel 24-bit color bitmap? Assuming the cost of this RAM in the 1980es was $5/KB what was the price of those two solutions? How much is it now?**
- How much video RAM is needed to supprot a 25 lines abu 80 rows character monochrome text screen:
    - A monochrome text screen typically uses 1 byte per character, where each character is represented by 1 byte of ASCII code.  
    - Total characters: 25 lines x 80 characters per line = 2,000 characters
    - RAM needed: Each character needs 1 byte  

        > 2,000 x 1 = 2000 bytes = **2 KB**
    
- How much for a 1024 x 768 pixel 24-bit color bitmap?
    - For a 24-bit color bitmap, each pixel is represented by 24 bits (3 bytes).
    - Total pixels: 1024 x 768 = 786,432 pixels

        > **RAM**: 786,432 x 3 = **2,359,296 bytes**

- Assuming the cost of this RAM in the 1980es was 5$/KB what was the price of those two solutions? How much is it now?
    - 1980s
        - Monochrome text screen:  

            > 2 KB x $5/KB = **$10**

        - 1024 x 768 pixel 24-bit color bitmap:  

            > 2,359,296 bytes = 2,304 KB  
            2,304 KB x $5/KB = **$11,520**

    - today: RAM costs **$0.002/MB**  
        - Monochrome text screen:  

            > 2 KB = 0.002 MB  
            0.002 MB x $0.002/MB = **$0.000004**

        - 1024 x 768 pixel 24-bit color bitmap:  

            > 2,359,396 bytes = 2.25 MB  
            2.25 MB x $0.002/MB = **$0.0045**
