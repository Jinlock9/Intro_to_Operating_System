# l5
## Author
- Name: **JINLOCK CHOI**
- Student No.: **520370990012**
- E-mail: **jinlock99@sjtu.edu.cn**

## Tasks
### 2 Layer programming
1. The program can be divided into three layers, what are they?

    - List structure (Linked list)
    - Sorting Algorithms
    - User Interface

2. Split the program into files according to the defined layers.
3. Create the appropriate corresponding header files.
4. If necessary rewrite functions such that no call is emitted from lower level functions to upper level functions.
5. The initial program implements a command line interface, write a “Menu interface” which (i) welcomes the user, (ii) prompts him for some task to perform, and (iii) runs it. When a task is completed the user should (i) be informed if it was successful and then (ii) be displayed the menu.From the menu he should be able to exit the program.

    - List structure (Linked list): list.c, list.h
    - Sorting Algorithms: sort.c, sort.h
    - User Interface: ui.c, ui.h

    - ex2/
        - list.c
        - list.h
        - sort.c
        - sort.h
        - ui.c
        - ui.h

6. Write two **main** functions, one which will “dispatch” the work to another function which will run the command line user interface and a second one which will “dispatch” the work to the Menu user interface.

    - main_cmd.c
    - main.menu.c

    - Compile: make all

### 3 Libraries
1. In order to understand libraries we first recall a few basics on compilation

    - What are the four stages performed when compiling a file?
    - Briefly describe each of them.

        1. Preprocessing

            - Transform the source code before acutal compilation.
            - Processing directives that start with a `#`.

        2. Compilation

            - Translate the preprocessed source code into assembly code.
            - The actual translation of the high-level source code into low-level assembly instructions for a specific architecture takes place.

        3. Assembly

            - Convert the assembly code into machine code.
            - The assembly code generated from the previous phase is converted into machine code by an assembler.

        4. Linking

            - Combine one or more object files and libraries to produce an executable program.
            - Takes multiple object files and libraries, resolves any external references, and generates an executable file.

2. A library is a collection of functions, data types, constants, etc. which are put together. When compiling, the machine code corresponding to those elements is generated. Two types of libraries exist: static and dynamic. Explain the difference between the two.

    > **Static libraries** are linked directly into the executable during compilation, resulting in a larger file but no external dependencies at runtime.

    > **Dynamic libraries**, on the other hand, are linked at runtime, keeping the executable smaller but requiring the library to be available separately when the program runs.

3. Generating a static library is a simple process: collect several functions and pack them into an `ar` archive.

    - Search more details on how to proceed.
    - Create two static libraries, one for each of the two lowest layers in the previous program.

        ```bash
        gcc -c list.c -o list.o
        gcc -c sort.c -o sort.o
        gcc -c ui.c -o ui.o

        ar rcs liblist.a list.o
        ar rcs libsort.a sort.o
        
        ```

    - Compile the command line version of the program using these two static libraries.

        ```bash
        gcc -c main_cmd.c -o main_cmd.o
        gcc -o main_cmd main_cmd.o -L. -lui -lsort -llist

        ./main_cmd
        ```

4. Generating shared, or dynamic, libraries is a slightly more complex process. Since the library is to be shared among various programs none of them can rely on a predefined location where to find the functions in the memory. Therefore as the library has to store its information at different memory addresses it is compiled into a Position-Independent Code (PIC). This is achieved by running gcc with the flag `-fpic`.   
Then in order to effectively create the dynamic library, gcc has to be re-run with the flag `-shared`.

    - Generate two dynamic libraries, one for each of the two lowest layers in the previous program.

        ```bash
        gcc -fpic -c list.c -o list.o
        gcc -fpic -c sort.c -o sort.o
        gcc -fpic -c ui.c -o ui.o

        gcc -shared -o liblist.so list.o
        gcc -shared -o libsort.so sort.o
        gcc -shared -o libui.so ui.o
        ```

    - Compile the whole program
    - Compile the Menu version of the program using these two dynamic libraries.

        ```bash
        gcc -o main_menu main_menu.c -L. -lui -lsort -llist

        export LD_LIBRARY_PATH=./:$LD_LIBRARY_PATH
        ./main_menu
        ```

5. A few extra remarks:
    - What is the difference between a library and the API.

        > A library is a collection of precompiled code that provides specific functionality, such as functions, classes, and data structures, which can be used by programs to perform tasks without writing the code from scratch. An API (Application Programming Interface) defines the set of rules, protocols, and tools for interacting with a library or system. The API specifies how the functions in a library can be used, including their inputs, outputs, and behavior, while the library implements those functions.

    - Implement the API below for the two libraries

        - ex3/
            - lab5_dlist.h
            - lab5_dlist.c