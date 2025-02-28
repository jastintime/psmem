.POSIX:
#PHONY became POSIX in 2024, implementations missing PHONY will just ignore it.
# gcc -Dmacro_name=definition for test when they get implemented.
.PHONY: clean all

CC = cc
CFLAGS = -O2 -g -Wall -Werror -Wextra -Winit-self -Wuninitialized -pedantic -ansi -D_DEFAULT_SOURCE -Wunreachable-code

OBJS = smaps.o psmem.o output.o prog_list.o

all: psmem

psmem: $(OBJS)
	$(CC) $(CFLAGS) -o psmem $(OBJS)

smaps.o: src/smaps.c src/psmem.h
	$(CC) $(CFLAGS) -c $<

psmem.o: src/psmem.c src/psmem.h
	$(CC) $(CFLAGS) -c $<

output.o: src/output.c src/psmem.h
	$(CC) $(CFLAGS) -c $<

prog_list.o: src/prog_list.c src/psmem.h
	$(CC) $(CFLAGS) -c $<

clean:
	-rm $(OBJS) psmem
