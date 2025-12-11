CC = c:\mingw-w64-7.0.0-i686\mingw32\bin\gcc.exe
CFLAGS = -Wall -Werror -O2

all: main.o
	$(CC) -o dateditor main.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c


.PHONY: clean

clean:
	-rm -f *.o *.exe
