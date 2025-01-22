CFLAGS=-Wall -Wextra -Wpedantic
SRCS=$(wildcard *.c)
OBJS=$(SRCS:%.c=%.o)
DEPS=$(SRCS:%.c=%.d)

PREFIX?=.
BINDIR=$(PREFIX)/bin
MANDIR=$(PREFIX)/share/man/man1

all: rmc rmc.1

rmc: $(OBJS)
	$(CC) -o $@ $^

debug: CFLAGS+=-g -O0 -Wno-cpp
debug: rmc

rmc.1: rmc.1.in
	sed 's/DATE/$(shell LC_TIME=en_US date '+%d %b %Y')/g' $< > $@

-include $(DEPS)

%.o: %.c
	$(CC) -MMD $(CFLAGS) -o $@ -c $<

clean:
	rm -f *.o *.d rmc

install:
	install -Dm755 rmc $(BINDIR)/rmc
	install -Dm644 rmc.1 $(MANDIR)/rmc.1

.PHONY: clean all install
