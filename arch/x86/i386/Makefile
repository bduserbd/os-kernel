OBJS += init.o
OBJS += cpu.o
OBJS += idt.o interrupt.o
OBJS += paging.o
OBJS += elf.o
OBJS += system-call.o
OBJS += spinlock.o
OBJS += task.o task-switch.o

CFLAGS += -I ../

.PHONY := all
all:
	$(V)$(MAKE) $(BUILD) TARGETS="$(OBJS)" CFLAGS="$(CFLAGS)"

