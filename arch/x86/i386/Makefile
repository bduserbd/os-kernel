OBJS += init.o
OBJS += cpu.o
OBJS += idt.o interrupt.o
OBJS += paging.o
OBJS += elf.o
OBJS += system-call.o

CFLAGS += -I ../

.PHONY := all
all:
	$(V)$(MAKE) $(BUILD) TARGETS="$(OBJS)" CFLAGS="$(CFLAGS)"

