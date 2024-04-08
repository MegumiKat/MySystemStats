CC = gcc
CFLAGS = -Wall
HEADER = header.h

all: a3

a3: main_functions.o stats_functions.o
	$(CC) $(CFLAGS) -o $@ $^	

.%o : %.c $(HEADER)
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	rm *.o
