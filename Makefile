# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -g

# Source files
SRCS = main_functions.c stats_functions.c

# Header files
HDRS = header.h

# Object files
OBJS = $(SRCS:.c=.o)

# Executable
TARGET = a3

# Rule to build executable from object files
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Rule to compile source files into object files
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c -o $@ $<

# Phony target to clean compiled files
clean:
	rm -f $(OBJS) $(TARGET)