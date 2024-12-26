# l9
## Device Driver
### Basic Questions
#### 1. What is a device driver?

A **device driver** in computing refers to a special kind of software program or a specific type of software application that controls a specific hardware device that enables different hardware devices to communicate with the computer's Operating System. A device driver communicates with the computer hardware by computer subsystem or computer bus connected to the hardware.

#### 2. Which system calls / file operations are needed to be implemented for character device?

1. `open()`:

    - The `open` operation is called when a process tries to open the device file.
    - `int (*open) (struct inode *, struct file *);`

2. `release()` (or `close()`):

    - This operation is called when the device file is closed.
    - `int (*release) (struct inode *, struct file *);`

3. `read()`:

    - The `read` operation allows data to be read from the character device to user space.
    - `ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);`

4. `write()`:

    - The `write` operation is called when data is sent to the device from user space.
    - `ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);`

5. `ioctl()`:

    - The `ioctl` (Input/Output Control) operation allows control commands to be sent to the device.
    - `long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);`

6. `poll()`:

    - This operation is used to check if the device is ready for `read` or `write` without blocking.
    - `unsigned int (*poll)(struct file *filp, struct poll_table_struct *wait);`

7. `llseek()`:

    - This operations is used to modify the file position for a character device.
    - `loff_t (*llseek) (struct file *, loff_t, int);`

8. `mmap()`:

    - This operation allows a character device to be mapped into user space memory.
    - `int (*mmap)(struct file *filp, struct vm_area_struct *vma);`

#### 3. What needs to be returned in `read` and `write` file operations of character device?

- `read()`:

    - On success, the number of bytes read is returned.

        - It is not an error if this number is smaller than the number of bytes requested, or the amount of data required for one frame.
        - This may happen for example because read() was interrupted by a signal.

    - On error, `-1` is returned, and the `errno` variable is set appropriately.

- `write()`:

    - On success, the number of bytes written are returned.

        - Zero indicates nothing was written.

    - On error, `-1` is returned, and the `errno` variable is set appropriately.

#### 4. What are major number and minor number? Try to explain with `ls -l /dev`.

- **Major number**: 
    
    The **major number** indentifies the driver associated with the device. Essentially, it tells the kernel which driver to use when accessing the device. Each type of device (like a hard disk, a serial port, or a keyboard) is assigned a unique major number that maps to a specific driver.

- **Minor Number**:

    The **minor number** is used by the driver to differentiate between different instances of the same type of device. For example, if there are multiple partitions on a disk, they all have the same major number (representing the disk driver) but different minor numbers to identify each partition.

- If we run `ls -l /dev`:

    ```cs
    jinlock@jinlock:~$ ls -l /dev
    total 0
    crw-r--r--  1 root    root     10, 235 Nov 28 13:27 autofs
    drwxr-xr-x  2 root    root         320 Nov 28 13:27 block
    drwxr-xr-x  2 root    root          80 Nov 28 13:27 bsg
    crw-rw----  1 root    disk     10, 234 Nov 28 13:27 btrfs-control

    ...

    brw-rw----  1 root    disk      8,   0 Nov 28 13:27 sda
    brw-rw----  1 root    disk      8,   1 Nov 28 13:27 sda1
    brw-rw----  1 root    disk      8,   2 Nov 28 13:27 sda2
    brw-rw----  1 root    disk      8,   3 Nov 28 13:27 sda3

    ...

    crw-rw-rw-  1 root    tty       5,   0 Nov 28 13:27 tty
    crw--w----  1 root    tty       4,   0 Nov 28 13:27 tty0
    crw-------  1 jinlock tty       4,   1 Nov 28 13:27 tty1

    ...
    ```

    - The first character indicates the type of the device file:
        - `c`: **character device** (e.h., `tty`) 
        - `b`: **block device** (e.g., `sda`) 

    - The two numbers in the output (separated by a comma) represent the major and minor numbers.
        - For example:
            - `tty0` (`4, 0`): This means the major number is 4 and the minor number is 0. The major number 4 corresponds to the terminal driver, and the minor number 0 identifies a specific terminal.
            - `sda` (`8, 0`): This means the major number is 8 and the minor number is 0. The major number 8 typically represents a SCSI disk or similar disk drive, and the minor number 0 indicates the whole disk.
            - `sda1` (`8, 1`): The major number 8 is the same as for sda, meaning it's handled by the same disk driver, but the minor number 1 specifies the first partition on that disk.

#### 5. Which command is used to create a char device file under `/dev` knowing the major number and minor number of the char device?

To create a character device file under `/dev` directory, use `mknod`:

```sh
sudo mknod /dev/<device_name> c <major_number> <minor_number>
```

#### 6. Where are the following terms located in linux source code?

- `module_init`, `module_exit`: **`include/linux/module.h`**
- `printk`: **`include/linux/printk.h`**
- `container_of`: **`include/linux/kernel.h`**
- `dev_t`: **`include/linux/types.h`**
- `MAJOR`, `MINOR`, `MKDEV`: **`include/linux/kdev_t.h`**
- `register_chrdev_region`: **`fs/char_dev.c`**
- `module_param`: **`include/linux/moduleparam.h`**
- `cdev_init`, `cdev_add`, `cdev_del`: **`include/linux/cdev.h`**
- `THIS_MODULE`: **`include/linux/export.h`**

## Tasks

### Questions

#### How to generate random numbers when working inside the Linux kernel? You think that a while back you read something about getting the current time.

Use `get_random_bytes` function that is commonly used in kernel space to generate random numbers.

```C
void get_random_bytes(void *buf, int nbytes);
```

#### How to define and specify module options?

- `module_init()`: A macro that defines which function should be called when the kernel module is loaded. This function is used for module initialization.

- **Loading Modules**:

    - `insmod <filename> [args]`: Loads a kernel module by filename, optionally passing arguments to it.
    - `modprobe <module_name> [parameter=value]`: Loads a kernel module and can also handle dependencies, allowing parameters to be specified for configuration.

    - Example:
        - `sudo insmod my_module.ko my_param=5`
        - `sudo modprobe my_module my_param=5`

The `module_init()` and `module_exit()` macros are used to manage what happens when modules are loaded and unloaded, ensuring proper initialization and cleanup.

### Source and Compilation