CFLAGS=-Wall -Wextra -Wpedantic
SRCS=client.c \
     escape.c \
     list_servers.c \
     main.c \
     message.c \
     opts.c \
     pidfile.c \
     queue.c \
     runtime_path.c \
     server.c \
     signals.c \
     spawn.c \
     term.c \
     usage.c
OBJS=$(SRCS:%.c=%.o)
DEPS=$(SRCS:%.c=%.d)
LDFLAGS=

WITH_LIBNOTIFY?=0
ifeq ($(WITH_LIBNOTIFY), 1)
	SRCS+=notify.c
	CFLAGS+=-DWITH_LIBNOTIFY $(shell pkg-config --cflags libnotify)
	LDFLAGS+=$(shell pkg-config --libs libnotify)
endif

PREFIX?=.
BINDIR=$(PREFIX)/bin
BASHCOMPDIR=$(PREFIX)/share/bash-completion/completions
MANDIR=$(PREFIX)/share/man/man1

all: rmc rmc.1

rmc: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

debug: CFLAGS+=-g -O0 -Wno-cpp
debug: rmc

rmc.1: rmc.1.scd
	scdoc < $(<) > $(@)

-include $(DEPS)

%.o: %.c
	$(CC) -MMD $(CFLAGS) -o $@ -c $<

clean:
	rm -f *.o *.d rmc

install:
	install -Dm755 rmc $(BINDIR)/rmc
	install -Dm644 rmc.1 $(MANDIR)/rmc.1
	install -Dm755 completions/rmc-completions.bash $(BASHCOMPDIR)/rmc

.PHONY: clean all install
