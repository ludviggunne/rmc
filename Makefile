CFLAGS=-Wall -Wextra -Wpedantic
SRCS=$(wildcard *.c)
OBJS=$(SRCS:%.c=%.o)
DEPS=$(SRCS:%.c=%.d)

PREFIX?=.
BINDIR=$(PREFIX)/bin

rmc: $(OBJS)
	$(CC) -o $@ $^

debug: CFLAGS+=-g -O0 -Wno-cpp
debug: rmc

-include $(DEPS)

%.o: %.c
	$(CC) -MMD $(CFLAGS) -o $@ -c $<

clean:
	rm -f *.o *.d rmc

install:
	install -Dm755 rmc $(BINDIR)/rmc

.PHONY: clean
