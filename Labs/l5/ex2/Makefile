CC = clang
CFLAGS = -std=gnu11 -O2 -Wall -Werror -Wconversion -pedantic -Wno-unused-result
MCFLAGS = -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined -fsanitize=integer
LAYER_SRC = list.c sort.c ui.c
MAIN_CMD = main_cmd.c
MAIN_MENU = main_menu.c
LAYER_H = *.h
CMD = cmd
MENU = menu

.PHONY: clean

all: $(CMD) $(MENU)

$(CMD): $(LAYER_SRC) $(LAYER_H)
	$(CC) $(CFLAGS) $(MCFLAGS) -o $(CMD) $(LAYER_SRC) $(MAIN_CMD)

$(MENU): $(LAYER_SRC) $(LAYER_H)
	$(CC) $(CFLAGS) $(MCFLAGS) -o $(MENU) $(LAYER_SRC) $(MAIN_MENU)

clean:
	$(RM) *.o *.a $(CMD) $(MENU) *.txt
