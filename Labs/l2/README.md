# ECE482 2024FA LAB 2
## Author
- Name: **JINLOCK CHOI**
- Student No.: **520370990012**
- E-mail: **jinlock99@sjtu.edu.cn**

## 2 Systems setup
### 2.1 Linux kernel compilation
- **What is a kernel?**  

    > **Kernel** is the layer between hardware and software, managing system resources like the CPU, memory, and devices. The kernel enables communication between hardware and software by controlling processes, memory, and peripheral devices like keyboards or printers.  

- **What basic tools are necessary to compile the Linux kernel?**
    - **C Compiler (gcc)**: The Linux kernel is written primarily in C, so the **gcc** is needed to build it.
    - **GNU Make**: This is required to process the kernel's `Makefile` and manage the build process.
    - **bc (Basic Calculator)**: A command-line calculator language used during the kernel build process.
    - **bison and flex**: These are used to generate parsers for the Linux kernel's configuration tools.
    - **libssl-dev (OpenSSL development libraries)**: Theses are required for compiling some kernel features like encryption.  

- **List all the commands necessary to compile the kernel.**
    ```
    # Step 0. Install the Required Tools for sched_ext

    sudo apt update
    sudo apt install build-essential libncurses-dev bison flex libssl-dev libelf-dev dwarves

    sudo apt install clang lld

    wget https://apt.llvm.org/llvm.sh
    chmod +x llvm.sh

    echo 'Acquire::https::apt.llvm.org::Verify-Peer "false";' | sudo tee /etc/apt/apt.conf.d/99disable-ssl-verification #(*)

    sudo ./llvm.sh 18
    sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-18 100
    sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-18 100

    # Step 1. Download the Kernel

    wget -v https://git.kernel.org/pub/scm/linux/kernel/git/tj/sched_ext.git/snapshot/sched_ext-for-6.11.tar.gz
    tar -zxvf sched_ext-for-6.11.tar.gz
    cd sched_ext-for-6.11

    # Step 2. Configure the Kernel

    cp /boot/config-$(uname -r) .config

    scripts/config --disable SYSTEM_TRUSTED_KEYS
    scripts/config --disable SYSTEM_REVOCATION_KEYS

    make CC=clang menuconfig

    # essential configs to enable sched_ext
    CONFIG_BPF=y
    CONFIG_SCHED_CLASS_EXT=y # Check this
    CONFIG_BPF_SYSCALL=y
    CONFIG_BPF_JIT=y
    CONFIG_BPF_JIT_ALWAYS_ON=y
    CONFIG_BPF_JIT_DEFAULT_ON=y
    CONFIG_PAHOLE_HAS_BTF_TAG=y
    CONFIG_DEBUG_INFO_BTF=y

    # useful debug features for sched_ext
    CONFIG_DEBUG_INFO=y
    CONFIG_SCHED_DEBUG=y
    CONFIG_DEBUG_INFO_DWARF5=y
    CONFIG_DEBUG_INFO_BTF_MODULES=y

    # Other useful options
    CONFIG_FRAME_WARN=2048 # Check this

    # Step 3. Compile the Kernel

    make CC=clang -j$(nproc)

    sudo make modules_install
    sudo make install

    # Step 4. Reboot

    sudo reboot

    # Step 5. Check the Kernel Version

    uname -r
    ```  

- **Why is it recommended to start with the Linux `config` file from your distribution?**  

    > Starting with one's distribution's `config` file is recommended because it simplifies the kernel configuration process, ensures hardware compatibility, maintains distribution-specific optimizations, and makes it easier to troubleshoot or update the kernel. It serves as a reliable base for customization while minimizing the risk of misconfigurations.  

- **Compile the kernel. How long did it take?**

    > **Approximately 4 hours**

- **Why is code quality so critical at the kernel level?**

    > Kernel-level code quality is critical because the kernel is the foundation of the entire operating system. High-quality code ensures system stability, performance, security, reliability, maintainability, proper hardware interfacing, effective concurrency management, and smooth interaction between user space and hardware.

### 2.2 Software Development
- **Briefly explain what is CI/CD.**

    - **Continuous Integration (CI)**: CI is the practice of regularly integrating code changes into a shared repository, where automated builds and tests are run.
    - **Continuous Delivery (CD)**: In Continuous Delivery, code changes are automatically build, tested, and prepared for release to production. While the code is always in a deployable state, actual deployment still requires manual approval. CD ensures that new features or fixes can be released quickly, reliably, and on-demand.
    - **Continuous Deployment (CD)**: This takes Continuous Delivery one step further by automatically deploying every code change that passes automated testing to production, without manual intervention. This allows for faster and more frequent updates to end-users.

- **Why is having a consistent coding style important in industry?**

    > A consistent coding style ensure clarity, efficiency, and long-term maintainability in professional software development environments

- **What is the benefit of sanitizers?**

    > Sanitizers enhance the reliability, security, and maintainability of software by identifying hard-to-detect bugs and issues during development.

## 3 Remote work
- **Setup and SSH server on Linux VM. From Linux (using `ssh`) or Windows (using `Putty`) log into it.**  

    ```
    vi ~/.ssh/config

    Host ve482
        HostName localhost
        Port 4823
        User jinlock
        IdentityFile "~/.ssh/id_rsa"

    ssh ve482
    ```  

- **What is the default SSH port? Change this port for port 2222. Log into your Linux VM using this new SSH server setup.**

    > **Port 22**

- **List and explain the role of each file in the `$HOME/.ssh` directory. In `$HOME/.ssh/config`, create an entry for the Linux VM.**

    - *config*: A configuration file to define various SSH client settings.
    - *id_rsa*: Private key for the RSA key pair.
    - *id_rsa.pub*: Public key for the RSA key pair.
    - *known_hosts*: Stores the public host keys of remote SSH servers that user has connected to.
    - *authorized_keys*: Contains a list of public keys that are allowed to authenticate to this user's account.

- **Briefly explain how key-only authentication works in SSH. Generate an ed25519 key-pair on the host system and use it to log into the VM without a password.**

    > Firstly, user generates a pair of key - a public key and a private key. SSH server receives a public key from user and checks whether the key is valid. Then the server marks the key as the authorized key and stores it. Therefore, user who possesses the private key is identified by checking its public pair authorized in the server, allowed to connect the server.

    ```
    ssh-keygen -t rsa
    ssh-copy-id ve482
    ```
## 4 Basic git
- Search the use of the following `git` commands:  
    - `help`: Provides information on Git commands.
    - `branch`: Lists, creates, or deletes branches in the repository.
    - `merge`: Integrates changes from one branch into another.
    - `tag`: Creates, lists, deletes, or verifies a tag.
    - `commit`: Records changes to the repository.
    - `init`: Initializes a new Git repository and begins tracking an existing directory.
    - `push`: Sends you local branch commits to a remote repository.
    - `add`: Adds changes in your working directory to the staging area.
    - `log`: Shows the commit history.
    - `clone`: Creates a copy of a remote repository on your local machine, maintaining the history, branches, and tags.
    - `checkout`: Switches between branches or specific commmits.
    - `pull`: Fetches the changes from a remote repository and merges them into the current local branch.
    - `diff`: Shows the differences between commits, branches, or your working tree and the repository.
    - `fetch`: Retrieves the changes from a remote repository but doesn't merge them.
    - `reset`: Resets your current HEAD to the specified state.