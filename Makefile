CC=$(shell which cc)
CCFLAGS=-std=gnu11 -Wall -Werror -pedantic
LD=$(shell which ld)
LDFLAGS=-lm

OBJS=$(shell php objs.php)
PROG=hello-vm

.PHONY: clean

all: $(OBJS)
	$(CC) $(CCFLAGS) -o $(PROG) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CCFLAGS) -c $<

clean:
	rm $(PROG)
	rm *.o
