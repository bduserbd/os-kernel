OBJS += init.o
OBJS += address-cache.o
OBJS += ipv4.o
OBJS += udp.o
OBJS += dhcp.o

CFLAGS = -I ../../

.PHONY: all
all:
	$(V)$(MAKE) $(BUILD) TARGETS="$(OBJS)" CFLAGS="$(CFLAGS)"

