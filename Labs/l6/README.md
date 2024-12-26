# l6

## Author
- Name: **JINLOCK CHOI**
- Student No.: **520370990012**
- E-mail: **jinlock99@sjtu.edu.cn**

## Tasks
### 3. Designing a plugin architecture
- Design a plugin architecture allowing to open various file-types.

    ```C
    typedef struct PlugIn {
        void *handle;
        char plugInType[40];
        void (*interface)(type_t*, char**);
    } plugIn_t;
    ```

- Refactor the code of the main application such that opening a text file becomes part of a plugin.

    - plugin_architecture/program/
        - main.c
        - list.c
        - list.h
        - plugin.c
        - plugin.h
        - sort.c
        - sort.h

- Adjust the code such that text files can opened and processed.

    - plugin_architecture/plugin-txt/
        - interface.c
        - interface.h

- Write the skeleton of a plugin to open and process csv files.
- Complete the implementation of the csv file plugin.

    - plugin_architecture/plugin-csv/
        - interface.c
        - interface.h

    - Compile: `make`
        - plugin_architecture/Makefile
    - Run: `./main [sort_data.txt / *.csv] [sorting_type]`
        - ex: `./main test/dec_int.txt rand` or `./main test/test.csv dec`

### 4. A real life software
- Write the skeleton of a simple Zathura plugin allowing to open a text file.
- Complete the plugin such that Zathura can open and display text files.

    - zathura-txt

        - `plugin.h`
        - `plugin.c`
        
    - build: 
    
        ```shell
        $ gcc -std=c99 -shared -fPIC -pedantic -Wall `pkg-config --cflags --libs poppler-glib zathura` -o zathura-txt.so plugin.c
        ```