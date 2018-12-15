OBJS += string.o
OBJS += bits.o
OBJS += divmod64.o
OBJS += bcd.o

CFLAGS = -I ../

.PHONY: all
all:
	$(V)$(MAKE) $(BUILD) TARGETS="$(OBJS)" CFLAGS="$(CFLAGS)"

