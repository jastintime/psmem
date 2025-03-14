.POSIX:
#PHONY became POSIX in 2024, implementations missing PHONY will just ignore it.
# gcc -Dmacro_name=definition for test when they get implemented.
.PHONY: clean all install install-man install-psmem

CC = cc
CFLAGS = -O2 -g -Wall -Werror -Wextra -Winit-self -Wuninitialized -pedantic -ansi -D_DEFAULT_SOURCE -Wunreachable-code
OBJS = smaps.o psmem.o output.o prog_list.o

DESTDIR = 
PREFIX = /usr/local
MANDIR = $(PREFIX)/share/man
BINDIR = $(PREFIX)/bin

all: psmem

install: install-man install-psmem

install-man:
	mkdir -p $(DESTDIR)$(MANDIR)/man1
	install -m644 psmem.1 $(DESTDIR)$(MANDIR)/man1/.

install-psmem: psmem
	mkdir -p $(DESTDIR)$(BINDIR)
	install -m755 psmem $(DESTDIR)$(BINDIR)/.

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
	-rm -f $(OBJS) psmem
