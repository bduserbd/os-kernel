OBJS += multiboot2.o
OBJS += main.o

CFLAGS += -Wall -m32
CFLAGS += -I ../

.PHONY: all
all:
	$(V)$(MAKE) $(BUILD) TARGETS="$(OBJS)" CFLAGS="$(CFLAGS)"

