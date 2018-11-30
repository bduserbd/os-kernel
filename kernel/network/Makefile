OBJS += network.o
OBJS += packet.o

CFLAGS = -I ../

.PHONY := all
all: compile dirs

compile:
	$(V)$(MAKE) $(BUILD) TARGETS="$(OBJS)" CFLAGS="$(CFLAGS)"

dirs:
	$(V)$(MAKE) -C ethernet/
	$(V)$(MAKE) -C ipv4/

