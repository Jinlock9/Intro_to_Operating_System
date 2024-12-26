# l11

## 3 Tasks
### 1. **What is a file system?**  
A file system is a method and data structure that the operating system uses to manage, store, retrieve, and organize data on storage devices. It provides a way to structure data into files and directories, enabling efficient access, modification, and storage.

---

### 2. **How is the Linux VFS working?**  
The **Linux Virtual File System (VFS)** is an abstraction layer that provides a unified interface to access different file systems in Linux.  

#### Workflow:
1. **Abstraction**: Applications interact with the VFS using standard system calls (e.g., `open`, `read`, `write`) without needing to know the underlying file system.
2. **File System Drivers**: The VFS forwards these calls to specific file system drivers (e.g., ext4, NFS, or FUSE) that translate operations for the actual file system.
3. **Mount Points**: Each file system is mounted into a directory hierarchy, allowing seamless navigation regardless of the physical storage device.

---

### 3. **What is FUSE, and how does it interact with the VFS?**  
**FUSE (Filesystem in Userspace)** allows users to create and run custom file systems in **user space** rather than kernel space. It provides an interface to interact with the VFS.

#### How it works:
1. **VFS Layer**: Applications perform file operations through the VFS.
2. **FUSE Kernel Module**: The VFS routes FUSE-backed requests to the FUSE kernel module.
3. **User Space Process**: The kernel module forwards these requests to a user space FUSE daemon or application, which processes them.
4. **Result**: The response is sent back to the kernel module and then to the application.

#### Sketch:
```
  Application
      |
  [VFS Layer]
      |
  [FUSE Kernel Module]
      |
  [FUSE User-space Daemon] <--> Custom File System
```
The key point is that the actual file system logic runs in user space, avoiding kernel modifications.

---

### 4. **What is SSHFS?**  
**SSHFS (SSH File System)** is a FUSE-based file system that allows mounting a remote file system over an **SSH connection**. It uses SSH to securely transfer file system operations between a local and remote machine.
In essence, it allows a remote directory to appear as a local directory, providing seamless file access.

---

### 5. **Use SSHFS to mount your home on Lemonion Inc. server (111.186.59.59):**  
You can mount your home directory using the following steps:

1. Ensure `sshfs` is installed on your machine:
   ```bash
   sudo apt-get install sshfs
   ```

2. Create a local mount point for the remote file system:
   ```bash
   mkdir ~/lemonion_home
   ```

3. Mount the remote directory using `sshfs`:
   ```bash
   sshfs 520370990012@111.186.59.59:/home/lemonion ~/lemonion_home
   ```

4. To unmount the directory later:
   ```bash
   fusermount -u ~/lemonion_home
   ```

Replace `your_username` with your actual username.

---

### 6. **What are the benefits of using SSHFS?**  
- **Secure**: Data transfer occurs over SSH, ensuring encryption and secure communication.  
- **Easy Setup**: No need for special server configuration; works with SSH-enabled servers.  
- **Cross-Platform**: SSHFS works on Linux, macOS, and Windows (with SSHFS-Win).  
- **Seamless Integration**: Mounted remote directories behave like local directories.  
- **No Admin Privileges**: Requires no root privileges on the server or client.  
- **Performance**: Efficient for low to moderate workloads due to SSH compression.  

---