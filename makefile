.POSIX:
.PHONY: clean test

CC = cc
CFLAGS = -O2 -Wall -Werror -Wextra -Winit-self -Wuninitialized -pedantic -ansi -D_DEFAULT_SOURCE 


psmem: smaps.o psmem.o output.o
	$(CC) $(CFLAGS) -g -o  psmem $^

%.o: src/%.c
	$(CC) $(CFLAGS) -c -g $<

clean:
	-rm *.o psmem
