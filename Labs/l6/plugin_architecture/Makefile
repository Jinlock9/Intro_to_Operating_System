CC = gcc
CFLAGS = -std=gnu11 -O2 -Wextra -Werror -pedantic -Wno-unused-result
PROGRAM_SRC = program/list.c program/main.c program/plugin.c
PLUGIN_TXT_SRC = program/list.c program/sort.c plugin-txt/interface.c 
PLUGIN_CSV_SRC = program/list.c program/sort.c plugin-csv/interface.c 
PROGRAM = main
PLUGIN_TXT = plugins/txt.plugin 
PLUGIN_CSV = plugins/csv.plugin
.PHONY: clean

all: $(PROGRAM_SRC) $(PLUGIN_TXT_SRC) $(PLUGIN_CSV_SRC)
	$(RM) *.o *.a $(PROGRAM) $(PLUGIN_TXT) *.txt *.csv
	$(CC) $(CFLAGS) -o $(PROGRAM) $(PROGRAM_SRC) -ldl
	$(CC) $(CFLAGS) -shared -fPIC -o $(PLUGIN_TXT) $(PLUGIN_TXT_SRC)
	$(CC) $(CFLAGS) -shared -fPIC -o $(PLUGIN_CSV) $(PLUGIN_CSV_SRC)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean: 
	$(RM) *.o *.a $(PROGRAM) $(PLUGIN_TXT) $(PLUGIN_CSV) *.txt *.csv

program: $(PROGRAM_SRC)
	$(CC) $(CFLAGS) -o $(PROGRAM) $(PROGRAM_SRC) -ldl

plugin_txt: $(PLUGIN_TXT_SRC)
	$(CC) $(CFLAGS) -shared -fPIC -o $(PLUGIN_TXT) $(PLUGIN_TXT_SRC)

plugin_csv: $(PLUGIN_CSV_SRC)
	$(CC) $(CFLAGS) -shared -fPIC -o $(PLUGIN_CSV) $(PLUGIN_CSV_SRC)

