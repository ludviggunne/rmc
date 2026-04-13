PREFIX	:=	.
CFLAGS	:=	-O3 -MMD
LDFLAGS	:=
SOURCES	:=	$(wildcard *.c)
OBJECTS	:=	$(SOURCES:%.c=%.o)
DEPENDS	:=	$(SOURCES:%.c=%.d)

rmc: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(@) $(^)

-include $(DEPENDS)

.c.o:
	$(CC) $(CFLAGS) -o $(@) -c $(<)

clean:
	rm -rf rmc *.o *.d

install:
	install -Dm755 rmc $(PREFIX)/bin/rmc

debug: CFLAGS += -O0 -g -Wno-cpp
debug: rmc

.PHONY: clean install debug
