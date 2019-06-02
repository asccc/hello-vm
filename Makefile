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

all: $(OBJS)
	$(CC) $(CCFLAGS) -o $(PROG) $(OBJS) $(LDFLAGS)

gen:
	php vm_gen.php

%.o: %.c
	$(CC) $(CCFLAGS) -c $<

test: all
	./$(PROG)

clean:
	rm $(PROG)
	rm *.o
	rm vm.inc
