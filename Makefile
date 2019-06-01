CC=$(shell which cc)
CCFLAGS=-std=gnu11 -Wall -Werror -Wno-missing-braces -pedantic
LD=$(shell which ld)
LDFLAGS=-lm

OBJS=$(shell php objs.php)
PROG=hello-vm

.PHONY: clean

all: $(OBJS)
	$(CC) $(CCFLAGS) -o $(PROG) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CCFLAGS) -c $<

test: all
	./$(PROG)

clean:
	rm $(PROG)
	rm *.o
