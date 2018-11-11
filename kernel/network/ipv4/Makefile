OBJS += init.o
OBJS += ipv4.o
OBJS += udp.o
OBJS += address-cache.o

CFLAGS = -I ../../

.PHONY: all
all:
	$(V)$(MAKE) $(BUILD) TARGETS="$(OBJS)" CFLAGS="$(CFLAGS)"

