# ECE482 2024FA LAB 4

## Author
- Name: **JINLOCK CHOI**
- Student No.: **520370990012**
- E-mail: **jinlock99@sjtu.edu.cn**

## 1 Database

### 1.1 Database creation

1. timestamp.csv

    `git log --pretty="%H,%aN,%aI,%at" > timestamp.csv`

2. db.csv

    `git log --pretty="%H,%aN,%s" > db.csv` 

### 1.2 Database system installation

1. What are the most common database systems?

    - MySQL
    - PostgreSQL
    - Oracle Database
    - Microsoft SQL Server
    - MongoDB

2. Briefly list the pros and cons of the three most common ones.

    - **MySQL**

        - Pros: Widely used, easy to set up, with strong performance for read-heavy
        applications.
        - Cons: Lacks some advanced features and isn't ideal for complex or large-scale
        analytics.

    - **PostgreSQL**

        - Pros: Feature-rich, highly extensible, and excellent for complex queries and large
        datasets.
        - Cons: Slower performance in simple applications and more resource-intensive.

    - **MongoDB**

        - Pros: Flexible schema, highly scalable, and fast for large volumes of 
        unstructed data.
        - Cons: Weaker ACID guarantees and less optimized for complex queries compared
        to SQL databases.

3. Create an empty SQLite database.

    `sqlite3 l4.db`

4. Use the SQLite shell to prepare two empty tables for each of your .csv file.

    ```js
    CREATE TABLE db (
        hash TEXT NOT NULL,
        name TEXT NOT NULL,
        comment TEXT NOT NULL  
    );

    CREATE TABLE time_stamp (
        hash TEXT NOT NULL,
        name TEXT NOT NULL,
        dates TEXT,
        tstamp INT
    );
    ```


5. Import each .csv file in its corresponding SQLite table.

    ```js
    .separator "|"
    .import ./lab4_datasets/db.psv db
    .import ./lab4_datasets/timestamp.psv time_stamp
    ```

### 1.3 Database queries
1. Who are the top five contributors to the Linux kernel since the beginning?

    ```js
    // INPUT
    SELECT name, count(name) AS freq FROM time_stamp
    GROUP BY name
    ORDER BY freq DESC
    LIMIT 5;

    // OUTPUT
    name            freq
    --------------  ----------
    Linus Torvalds  30702
    David S. Mille  13180
    Takashi Iwai    7726
    Mark Brown      7670
    Arnd Bergmann   7520
    ```  

2. Who are the top five contributors to the Linux kernel for each year over the past five years?

    **NO INFORMATION BETWEEN 2021 - 2024**

    - **2020**

        ```js
        // INPUT
        SELECT name, count(name) AS freq FROM time_stamp
        WHERE dates BETWEEN '2020-01-01' AND '2020-12-31'
        GROUP BY name
        ORDER BY freq DESC
        LIMIT 5;
        
        // OUTPUT
        name            freq
        --------------  ----------
        Linus Torvalds  1886
        David S. Mille  923
        Christoph Hell  806
        Mauro Carvalho  770
        Chris Wilson    644
        ```  

    - **2019**

        ```js
        // INPUT
        SELECT name, count(name) AS freq FROM time_stamp
        WHERE dates BETWEEN '2019-01-01' AND '2019-12-31'
        GROUP BY name
        ORDER BY freq DESC
        LIMIT 5;

        // OUTPUT
        name            freq
        --------------  ----------
        Linus Torvalds  2380
        David S. Mille  1205
        Chris Wilson    1170
        YueHaibing      929
        Christoph Hell  911
        ```  

    - **2018**

        ```js
        // INPUT
        SELECT name, count(name) AS freq FROM time_stamp
        WHERE dates BETWEEN '2018-01-01' AND '2018-12-31'
        GROUP BY name
        ORDER BY freq DESC
        LIMIT 5;

        // OUTPUT
        name            freq
        --------------  ----------
        Linus Torvalds  2163
        David S. Mille  1405
        Arnd Bergmann   919
        Christoph Hell  818
        Colin Ian King  798
        ```  
    
    - **2017**

        ```js
        // INPUT
        SELECT name, count(name) AS freq FROM time_stamp
        WHERE dates BETWEEN '2017-01-01' AND '2017-12-31'
        GROUP BY name
        ORDER BY freq DESC
        LIMIT 5;

        // OUTPUT
        name            freq
        --------------  ----------
        Linus Torvalds  2288
        David S. Mille  1420
        Arnd Bergmann   1123
        Chris Wilson    1028
        Arvind Yadav    827
        ``` 

    - **2016**

        ```js
        // INPUT
        SELECT name, count(name) AS freq FROM time_stamp
        WHERE dates BETWEEN '2016-01-01' AND '2016-12-31'
        GROUP BY name
        ORDER BY freq DESC
        LIMIT 5;

        // OUTPUT
        name            freq
        --------------  ----------
        Linus Torvalds  2273
        Arnd Bergmann   1185
        David S. Mille  1150
        Chris Wilson    988
        Mauro Carvalho  975
        ```  

3. What is the most common "commit subject"?

    ```js
    // INPUT
    SELECT comment, count(comment) AS freq FROM db
    GROUP BY comment
    ORDER BY freq DESC
    LIMIT 1;

    // OUTPUT
    comment                                                        freq
    -------------------------------------------------------------  ----------
    Merge git://git.kernel.org/pub/scm/linux/kernel/git/davem/net  670
    ```

4. On which day is the number of commits the highest?

    ```js
    // INPUT
    SELECT date(dates) AS date, count(dates) AS freq FROM time_stamp
    GROUP BY date
    ORDER BY freq DESC
    LIMIT 1;

    // OUTPUT
    date        freq
    ----------  ----------
    2008-01-30  1031
    ```

5. Determine the average time between two commits for the five main contributors.

    **CHECK avg COLUMN**  

    ```js
    // INPUT
    SELECT name, count(name) AS freq, MAX(tstamp) AS latest, MIN(tstamp)AS earliest, (MAX(tstamp) - MIN(tstamp)) / (count(name) - 1) AS avg FROM time_stamp
    GROUP BY name
    ORDER BY freq DESC
    LIMIT 5;

    // OUTPUT
    name            freq        latest      earliest    avg
    --------------  ----------  ----------  ----------  ----------
    Linus Torvalds  30702       1601242690  1113690036  15880
    David S. Mille  13180       1600735253  1113690241  36956
    Takashi Iwai    7726        1600683992  1111682910  63301
    Mark Brown      7670        1598466419  1138838401  59933
    Arnd Bergmann   7520        1596184915  1116420059  63807
    ```

## 2 Debugging
1. How to enable built-in debugging in gcc?

    > By using -g flag (ex: `gcc -g -o output source.c`).

2. What is the meaning of GDB?

    > **GDB** stands for **GNU Project Debugger**. GDB is widely used in debugging low-level
    programs, especially those written in C and C++, as it offers granular control 
    over program execution.

3. Compile the master branch of your `mumsh` with debugging enabled.

    `gcc -g -o mumsh mumsh.c func.c utils.c`

### 2.1 Basic GDB usage

1. Find the homepage of the GDB project.

    **https://www.sourceware.org/gdb/**

2. What languages are supported by GDB?

    - Ada
    - Assembly
    - C
    - C++
    - D
    - Fortran
    - Go
    - Objective-C
    - OpenCL
    - Modula-2
    - Pascal
    - Rust

3. What are the following GDB commands doing:

    - **backtrace**: Displays the call stack of function calls leading to the current execution point.

    - **delete**: Removes breakpoints or watchpoints that were previously set.

    - **where**: An alias for `backtrace`, showing the current call stack.

    - **info breakpoints**: Lists all active breakpoints, including their locations and conditions.

    - **finish**: Runs the program until the current function returns, allowing you to see the return value or where the program proceeds next.

4. Search the documentation and explain how to use conditional breakpoints.

    1. Set the breakpoints:

        `(gdb) break function_name` or `(gdb) break filename:line_number`

    2. Add a condition:

        `(gdb) condition breakpoint_number expression`

5. What is `-tui` option for GDB?

    > The `-tui` option in GDB stands for Text User Interface. 
    When you run GDB with the -tui option, it opens a split-screen interface 
    that shows the source code alongside the usual GDB command-line interface.

6. What is the "reverse step" in GDB and how to enable it? Provide the key steps and commands.

    1. Start GDB:

        `gdb program_name`

    2. Enable Reverse Execution:

        - `reverse-stepi`: Steps backward by a single machine instruction  

            `(gdb) reverse-stepi`

        - `reverse-step`: Steps backward to the previous line of code in the current function

            `(gdb) reverse-step`

        - `reverse-continue`: Run the program in reverse until a breakpoint or condition is met

            `(gdb) reverse-continue`

## 2.2 Basic LLDB usage
1. Quickly go through https://lldb.llvm.org/use/map.html to understand how to transition from *gdb* to LLDB;

    - Commands Mapping

        - GDB: `run` - LLDB: `process launch`
        - GDB: `break` - LLDB: `breakpoint set`
        - GDB: `next` - LLDB: `thread step-over`
        - GDB: `continue` - LLDB: `process continue`

2. Follow LLDB tutorial presented at https://lldb.llvm.org/use/tutorial.html;

    - Launching LLDB:
     
        `lldb executable_name`

    - Setting Breakpoints:

        `(lldb) breakpoint set --name main`

    - Running the Program:

        `(lldb) process launch`

    - Stepping Through Code:

        - `(lldb) thread step-over`
        - `(lldb) thread step-in`

    - Inspecting Variables:

        `(lldb) frame variable`