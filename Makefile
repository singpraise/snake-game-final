CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -Wpedantic -O0 -g -finput-charset=UTF-8 -fexec-charset=UTF-8
TARGET  = snake
SRCS    = main.c init.c snake.c queue.c stack.c board.c memory.c game_ui.c game_config.c obstacles.c score.c audio.c level.c editor.c input_win.c
OBJS    = $(SRCS:.c=.o)

.PHONY: all clean run debug

all: $(TARGET)

debug: CFLAGS += -DMEM_DEBUG
debug: clean $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) snake.exe
