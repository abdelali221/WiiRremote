CC = gcc
CFLAGS = -Wall -Werror -O2
TARGET = DATEditor

all: main.o
	$(CC) -o $(TARGET) main.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c


.PHONY: clean

clean:
	-rm -f *.o $(TARGET)
