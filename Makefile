.POSIX:
#PHONY became POSIX in 2024, implementations missing PHONY will just ignore it.
.PHONY: clean all

CC = cc
CFLAGS = -O2 -Wall -Werror -Wextra -Winit-self -Wuninitialized -pedantic -ansi -D_DEFAULT_SOURCE -Wunreachable-code

OBJS = smaps.o psmem.o output.o

all: psmem

psmem: $(OBJS)
	$(CC) $(CFLAGS) -g -o psmem $(OBJS)

smaps.o: src/smaps.c src/psmem.h
	$(CC) $(CFLAGS) -c -g $<

psmem.o: src/psmem.c src/psmem.h
	$(CC) $(CFLAGS) -c -g $<

output.o: src/output.c src/psmem.h
	$(CC) $(CFLAGS) -c -g $<

clean:
	-rm $(OBJS) psmem
