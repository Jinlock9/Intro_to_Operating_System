# h8

## ECE4820J Exercises

### Ex. 1 Input/Output

#### 1. Is it possible to scan a document and send it through a wireless network at full speed? Explain why, why not.

> Achieving full wireless transmission speed while scanning a document is theoreticallyy possible if the processes for scanning (calculation) and date transmission (I/O) are completely independent. 
> However, in practice, constraints like buffer management, processing overhead, and hardware limitations often introduce delays. Additionally, wireless network factors such as protocol overhead, congestion, and interference can futher reduce effective transmission speed.

#### 2. How can an OS facilitate the installation of a new device without any need to recompile the OS or even to reboot it?

> An OS facilitates new device installation without recompiling or rebooting by using **loadable drivers**, which can be dynamically loaded at runtime. 

#### 3. In the slides, four I/O software layers are listed; in which layers are the following operations done:

- (a) Computing the track, sector, and head for a disk read: **Device-Independent Software**
- (b) Writing command to the device registers: **Device Driver Layer**
- (c) Checking if the user is allowed to use the device: **User-Level I/O Software (Interrupt Handlers)** 
- (d) Converting binary integers to ASCII for printing: **User-Level I/O Software**

#### 4. A DMA controller has four channels. The controller is able to request a 32 bit word every 100 nsec. A respond takes equally long. How fast should the bus be in order to avoid being a bottleneck?

- Given:
    - Number of channels: 4
    - Request time per channel: 100 ns
    - Response time per channel: 100ns
    - Data size per transaction: 32 bits

- Each channel requires $100 ns + 100 ns = 200 ns$ for a complete transaction.
    - Each channel transfers *4 bytes every 200 ns*: *4 bytes / 200 ns = 20 MB/s*

- Total Bandwidth = 20 MB/s per channel x 4 channels = **80 MB/s**

#### 5. Briefly explain what a thin client is.

> A **thin client** is a lightweight computing device that depends on a central server for most of its processing, storage, and resource management. It is typically optimized for remote connections and designed to provide access to the server's resource with minimal hardware requirements.

#### 6. Describe how a hard disk is working and give a few details regarding its geometry.

> A **hard disk** consists of several spinning **platters** coated with magnetic material to store data. **Read/Write heads** are used to access data on the platters, which are divided into **tracks** (concentric circles). Each track is further divided into **sectors**, the smallest addressable storage unit. The combination of all tracks at the same position across multiple platters forms a **cylinder**, which is used for organizing data.

#### 7. What are differences between RAID 0, 1, 5, 6, and 10?

- **RAID 0**:
    - Data is split into blocks and striped across all drives, providing the fastest read and write speeds.
    - **No fault tolerance**: If one disk fails, all data is lost.
    - **Minimum drives required**: 2
- **RAID 1**:
    - Data is mirrored across at least two drives, providing full redundancy.
    - Fault-tolerant: Can survive the failure of one drive.
    - Performance: Good read speed (reads can be parallelized), moderate write speed.
    - **Minimum drives required**: 2
- **RAID 5**:
    - Combines striping with parity, distributing parity data across all drives for fault tolerance.
    - Can survive the failure of **one drive**.
    - Performance: Good read speed; slower writes due to parity calculations.
    - **Minimum drives required**: 3
- **RAID 6**:
    - Similar to RAID 5 but uses **dual parity** for additional redundancy.
    - Can survive the failure of **two drives**.
    - Performance: Slower writes than RAID 5 due to dual parity calculations.
    - **Minimum drives required**: 4
- **RAID 10**:
    - Combines **mirroring (RAID 1)** and **striping (RAID 0)**. Data is mirrored first, then striped for performance.
    - Can tolerate multiple disk failures as long as no mirrored pair is lost.
    - Performance: Excellent read and write speeds due to striping and redundancy.
    - **Minimum drives required**: 4

### Ex. 2 Multiprocessors

#### 1. Suppose that the `TSL` instruction was not available for synchronizing a multiprocessor. Instead, another instruction, `SWP` was provided that atomically swapped the contents of a register with a word in memory. How could that be used to provide multiprocessor synchronization?

> The `SWP` instruction can be used to implement multiprocessor synchronization by creating a spinlock mechanism. A shared memory varaible (e.g., `LOCK`) is initialzied to `0` to indicate an unlocked state. To acquire the lock, the thread loads `1` into a register and uses the `SWP` instruction to atomically exchange the register's value with the `LOCK` variable. If the previous value of `LOCK` is `0`, the thread acquires the lock; otherwise, it retries until the lock becomes available. To release the lock, the thread writes `0` back into the `LOCK` varaible. This ensures mutual exclusion by leveraging the atomicity of `SWP`. However, busy-waiting and potential starvation must be considered when implementating this approach.

#### 2. Briefly describe virtualization and the main challenges that need to be overcome.

> Virtualization is a technology that uses software to emulate the functionality of hardware, allowing multiple virtual machines (VMs) to run on a single physical machine. It abstracts and allocates hardware resources like CPU, memory, storage, and I/O to create isolated environments for each VM. The primary challenges include managing performance overhead introduced by virtualization layers, ensuring efficient resource allocation among VMs, maintaining strong isolation and security to prevent data breaches or interference, and handling hardware compatibility. Additional concerns include minimizing I/O latency, leveraging hardware-assisted virtualization features, and ensuring reliable data backup and disaster recovery.

### Ex. 3 File systems

#### 1. Assuming the current working directory is `/usr/share/doc/mutt/examples` what is the absolute path of `../../../doc/../../lib/mutt`?

`/usr/lib/mutt`

#### 2. A Unix system has 1KB blocks and 4B disk addresses. What is the maximum file size if i-nodes contain 13 direct entries, and one single, double, and triple indirect entry each?

- Given:
    - Block size = 1 KB
    - Disk address size = 4 bytes
        - Each block can store 256 addresses
    - i-node contains: 13 direct entries, 1 single indirect, 1 double indirect, 1 triple indirect.

- Direct blocks:
    
    *13 blocks x 1 KB/block = 13 KB*

- Single indirect block:

    *256 addresses x 1 KB/block = 256 KB*

- Double indirect block:

    *256 single indirect blocks x 256 addresses/block x 1 KB/block = 65,536 KB = 64 MB*

- Triple indirect block:

    *256 double indirect blocks x 256 single indirect blocks x 256 addesses/block x 1 KB/block = 16,777,216 KB = 16 GB*

- **Total**:

    *Total = 13 KB + 256 KB + 65,536 KB + 16,777,216 KB = 16,842,021 KB*

    The maximum file size is **16.84 GB**

#### 3. The time required to read a certain number of bytes can be approximated by summing the seek, rotation delay, and transfer times. Given a disk with mean seek time of 8 msec, a rotation rate of 15,000 rpm and 262,144 bytes per track what are the data rates for block sizes of (i) 1 KB, (ii) 2 KB, and (iii) 4 KB?

- Given:
    - Mean seek time: 8 ms
    - Rotation rate = 15,000 rpm
        - Time per rotation: 1 min / 15,000 = 4 ms
        - Mean rotational delay = (Time per rotation) / 2 = 2 ms
    - Bytes per track = 262,144 bytes
        - Transfer time per track = (Time per rotation) / (Bytes per track) = 4 ms / 262,144 = 0.00001526 ms/byte

1. 1 KB (1,024 bytes):
    - Seek time = 8 ms
    - Rotational delay = 2 ms
    - Transfer time = 1.024 x 0.00001526 = 0.01564 ms
    - Total time = 8 + 2 + 0.01564 = 10.01564 ms
    - **Data rate** = 1,024 / 10.01564 = **102.3 bytes/ms**

2. 2 KB (2,048 bytes):
    - Seek time = 8 ms
    - Rotational delay = 2 ms
    - Transfer time = 2,048 x 0.00001526 = 0.03128 ms
    - Total time = 8 + 2 + 0.03128 = 10.03128ms
    - **Data rate** = 2,048 / 10.031284 = **204.2 bytes/ms**

3. 4 KB (4,096 bytes):
    - Seek time = 8 ms
    - Rotational delay = 2 ms
    - Transfer time = 4,096 x 0.00001526 = 0.06256 ms
    - Total time = 8 + 2 + 0.06256 = 10.06256 ms
    - **Data rate** = 4,096 / 10.06256 = **407.0 bytes/ms**

### Ex. 4 Security

#### 1. If using only lowercase letters, how many possible 8 letter words can be chosen? Explain why a password should always contain lower and uppercase letters, number and symbols.

> While 8-character passwords with lowercase letters only have a large number of combinations (**208,827,064,576 possible words**), adding uppercase letters. numbers, and symbols significantly enhances by increasing to the total number of possibilities and protecting against brute-force and dictionary attacks.

#### 2. Explain why memory dyanamically allocated using `malloc` is sometimes random and some other times set to 0.

> Memory allocated using `malloc` can appear **random** because it retains leftover data from previous uses, potentially exposing sensitive information, such as passwords or private data, from other processes or parts of the program. However, it may sometimes appear **set to 0** due to moderen operating systems zeroing out memory for **security reasons**, preventing data leakage between processes.

#### 3. A computer system is made of several components each featuring a different level of security. How secure is this system?

> A computer system is only as secure as its weakest component.

#### 4. Can security be achieved? Explain your answer.

> While **absolute security** is unattainable due to the dynamic nature of threats and human limitations, **practical security** can be achieved by managing risks, implementing strong defenses, and staying proactive against emerging threats. Security is an ongoing process, not a one-time goal.

### Ex. 5 Research

#### Write about half a page on the topic of firewalls on Linux systems; include informatin on `iptables`. Do not forget to reference your sources of information.

**Firewalls** are critical components of network security on Linux systems, acting as barriers that monitor and control incoming and outgoing network traffic based on predetermined security rules. Among the various firewall solutions available for Linux, `iptables` has been a longstanding and widely used tool for managing network traffic.

##### Overview of `Iptables`

`Iptables` operates at the packet level, allowing system administrators to define rules that dictate how packets should be handled. It utilizes a series of tables and chains, where each table contains different types of chains that manage specific traffic flows: INPUT for incoming packets, OUTPUT for outgoing packets, and FORWARD for packets being routed through the system. Each rule within these chains can permit, deny, or modify packets based on criteria such as source IP address, destination port, or protocol type [2][3].

##### Key Features of `Iptables` [1][5]

- Packet Filtering: Iptables can filter packets based on various attributes, ensuring that only legitimate traffic is allowed through.
- Network Address Translation (NAT): This feature allows iptables to modify packet headers to enable communication between different networks.
- Logging: Administrators can enable logging to monitor traffic and detect unusual activities

##### Configuration and Management 

Configuring iptables involves using command-line instructions to add or modify rules. For example, to allow SSH traffic, an administrator might use:
```bash
sudo iptables -A INPUT -p tcp --dport 22 -j ACCEPT
```
This command appends a rule to the INPUT chain that accepts TCP packets directed to port 22 (the default SSH port) [4][5].

Despite its robustness, iptables can be complex for users unfamiliar with its syntax and structure. To address this complexity, several front-end tools have emerged. For instance, UFW (Uncomplicated Firewall) provides a more user-friendly interface for managing iptables rules on Ubuntu systems, while firewalld offers dynamic management capabilities and is often used in Red Hat-based distributions [2][4].

##### Conclusion

While iptables remains a powerful tool for firewall management in Linux environments, its complexity has led to the development of more accessible alternatives like UFW and firewalld. Regardless of the tool chosen, effective firewall management is essential for protecting Linux systems from unauthorized access and cyber threats [3][4].

##### References
[1] https://www.zenarmor.com/docs/network-security-tutorials/best-linux-firewall-solutions-in-2022  
[2] https://www.pluralsight.com/resources/blog/software-development/linux-firewall-administration  
[3] https://tuxcare.com/blog/linux-firewalls-enhancing-security-with-system-services-and-network-protocols/  
[4] https://www.ninjaone.com/blog/how-to-configure-a-linux-firewall/  
[5] https://www.geeksforgeeks.org/linux-firewall/

## ECE4821J Exercise

### Ex. 6 Research

#### 1. Write about half a page on the topic of firewalls on Linux systems; `ebtables` and `arptables`. Do not forget to reference your sources of information.

**Firewalls** are critical components in securing Linux systems, and among the tools available for managing network traffic are `ebtables` and `arptables`. Both utilities serve specific roles within the Linux networking stack, particularly in the context of bridging and ARP (Address Resolution Protocol) traffic.

##### `ebtables`  

`ebtables` is designed for filtering Ethernet frames at the data link layer (Layer 2) of the OSI model. It allows system administrators to define rules that control the flow of Ethernet packets between network interfaces, making it particularly useful in environments that utilize bridging. This utility can filter traffic based on MAC addresses, VLAN tags, and other Ethernet-specific criteria. For instance, it can be used to prevent certain devices from communicating with each other on a local network, thereby enhancing security by isolating traffic [1][2].

##### `arptables`

On the other hand, `arptables` focuses specifically on ARP packets. Since ARP is used to map IP addresses to MAC addresses within a local network, `arptables` allows administrators to set rules that can block or allow ARP requests and replies. This capability is essential for preventing ARP spoofing attacks, where a malicious actor sends falsified ARP messages over a local area network. By employing `arptables`, administrators can ensure that only legitimate ARP traffic is processed, thus protecting the integrity of network communications [1][3].

##### Reference 
[1] https://fmyson.tistory.com/231  
[2] https://tuxcare.com/blog/linux-firewalls-enhancing-security-with-system-services-and-network-protocols/  
[3] https://learn.saylor.org/mod/book/view.php?id=29908  

#### 2. As Linux is migrating from `iptables` to `nftables`, briefly present the latter one and explains it benefits over the former one.

> `nftables` is a modern replacement for `iptables`, providing a unified framework for managing IPv4, IPv6, ARP, and Ethernet filtering. It offers a simplified syntax, better performance through optimized data structures, and reduced kernel-space updates, improving scalability and efficiency. Its modular design makes it extensible for modern protocols, while tools like `iptables-nft` ensure backward compatibility. These advantages make `nftables` a more efficient and powerful solution for firewall management on Linux systems.