CC = gcc 
CFLAGS = -Wall -Wextra -pedantic
SRCS = $(shell find src -name '*.c')
OBJS = $(SRCS:%.c=%.o)
INCLUDE_DIRS = $(shell find src -type d)
INCLUDES = $(addprefix -I,$(INCLUDE_DIRS))

TARGET = ./file2c

.PHONY: all
all: $(TARGET)

.PHONY: run
run: $(TARGET)
	$(TARGET)

.PHONY: clean
clean:
	rm -f $(TARGET)
	rm -f $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)
