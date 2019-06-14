CC=$(shell which cc)
CCFLAGS=-std=gnu11 -Wall -Werror -Wno-missing-braces \
	-Wno-unused-function -pedantic

LDFLAGS=-lm

OBJS=$(shell php -r '                             \
	echo implode(" ", array_map(function ($$file) { \
		return basename($$file, ".c") . ".o";         \
	}, glob(__DIR__ . "/*.c")));                    \
')

PROG=hello-vm

.PHONY: clean

all: CCFLAGS+=-DNDEBUG -O3
all: gen comp

dbg: CCFLAGS+=-g
dbg: gen comp

gen:
	php vm_gen.php

comp: $(OBJS)
	$(CC) $(CCFLAGS) -o $(PROG) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CCFLAGS) -c $<

test: all
	./$(PROG)

clean:
	rm -f $(PROG)
	rm -f *.o
	rm -f vm.inc
