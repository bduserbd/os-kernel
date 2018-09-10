OBJS += multiboot2.o
OBJS += idt.o
OBJS += main.o cpu.o paging.o

CFLAGS += -I ../

.PHONY: all
all:
	$(V)$(MAKE) $(BUILD) TARGETS="$(OBJS)" CFLAGS="$(CFLAGS)"

