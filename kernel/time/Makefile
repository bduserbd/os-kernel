OBJS += device.o
OBJS += clock.o
OBJS += timer.o
OBJS += cmos.o

CFLAGS = -I ../

.PHONY: all
all:
	$(V)$(MAKE) $(BUILD) TARGETS="$(OBJS)" CFLAGS="$(CFLAGS)"

