# ECE482 2024FA LAB 3
## Author
- Name: **JINLOCK CHOI**
- Student No.: **520370990012**
- E-mail: **jinlock99@sjtu.edu.cn**

## 2. Source code
### 2.1 Linux kernel walk-through

### 2.2 The `diff` and `patch` commands
- Read the manpages of `diff` and `patch`

    ```
    man diff
    man patch
    ```

- Edit a file of your choice in `/usr/src`, e.g. add a comment to a file

    ```
    #1 on Linux Virtual Machine

    cp -r /usr/src /usr/src_orig

    #2 on Linux Virtual Machine

    echo "test" > /usr/src/test.txt
    echo "test_patched" > /usr/src_orig/test.txt

    #3 on my Linux system

    scp -r jinlock:/usr/src ./usr
    ```

- Using the `diff` command, create a patch corresponding to the above changes

    ```
    # on Linux Virtual Machine

    diff -aurN /usr/src /usr/src_orig > patch
    ```

- Retrieve your patch on your Linux system

    ```
    # on my Linux system

    scp -r jinlock:~/patch .
    ```

- Apply your patch to the copy of `/usr/src_orig` on your Linux system

    ```
    # on my Linux system

    patch -p3 ./usr/src/test.txt < patch
    ```

- Revert the patch

    ```
    # on my Linux system

    patch -R ./usr/src/test.txt < patch
    ```

### 2.3 Basis git usage
- Main  

    ![Main](img/l3-1.png "Main")  

- Remote  
  
    ![Remote](img/l3-2.png "Remote")

## 3. Scripting and regular expressions
Two programming languages often used in conjunction with Bash are `sed` and `awk`.
- Pipelining the output of `ifconfig` to `awk` return only the ip address of your current active network connection (the active network interface can be passed to `ifconfig`).  

    `ifconfig | awk '{ for (i = 1; i <= NF; i++) { if ($i == "inet") { print $i+1 } } }`

## 4. `rsync` command
In Unix-like systems the `rsync` program allows to synchronize different folders 
on the same system or over the network. When applying some changes to the source 
code it is highly recommended to have a copy of the original version such as to 
be able to revert back to the previous version in case of problem.
- Install the `rsync` software on both the host and Linux guest

    `sudo apt install rsync`

- Read `rsync` manpage

    `man rsync`

- Explain the benefit of `rsync` compared to a regular `cp`

    - Incremental Transfer: Copies only changed data, saving time and bandwidth
    - Network Support: Easily handles remote transfers over SSH.
    - File Attribute Preservation: Preserves permissions, ownership, and timestamps
    by default.
    - Error Recovery: Can resume interrupted transfers.

- How could you use `rsync` to transfer files across devices?

    `rsync [options] source destination`

    - Transfer from Local to Remote:

        `rsync -avz ./path jinlock:~/path`

    - Transfer from Remote to Local:

        `rsync -avz jinlock:~/path ./path`

## 5. More regular expressions
Using `curl` or `wget` retrieve information on shanghai air quality and pipe it 
to `sed` which should parse the output in order to display the information in the 
terminal following the format below  
`AQ: value Temp: value ºC (e.g. AQ: 55 Temp: 24 ºC).`

    curl -s "https://aqicn.org/?city=Shanghai&widgetscript&size=large&id=52b39d71decf07.20261781" | sed -n 's/.*id=.hdrpm10.*>\([0-9]\+\).*id=.hdrt.*>\([0-9]\+\).*/AQ: \1 Temp: \2 ºC/p'

    Result : AQ: 55 Temp: 21 ºC
