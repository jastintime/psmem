.POSIX:
.PHONY: clean all

CC = cc
CFLAGS = -O2 -Wall -Werror -Wextra -Winit-self -Wuninitialized -pedantic -ansi -D_DEFAULT_SOURCE -Wunreachable-code

all: psmem

psmem: smaps.o psmem.o output.o
	$(CC) $(CFLAGS) -g -o psmem $^

smaps.o: src/smaps.c src/psmem.h
	$(CC) $(CFLAGS) -c -g $<

psmem.o: src/psmem.c src/psmem.h
	$(CC) $(CFLAGS) -c -g $<

output.o: src/output.c src/psmem.h
	$(CC) $(CFLAGS) -c -g $<

clean:
	-rm *.o psmem
