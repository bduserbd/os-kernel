OBJS += slab.o
OBJS += buddy.o
OBJS += zone.o

CFLAGS = -I ../

.PHONY: all
all:
	$(V)$(MAKE) $(BUILD) TARGETS="$(OBJS)" CFLAGS="$(CFLAGS)"

