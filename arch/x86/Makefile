OBJS += init.o
OBJS += cpu.o mp.o lapic.o
OBJS += idt.o interrupt.o
OBJS += video.o

CFLAGS += -I ../

.PHONY := all
all: compile dirs

compile:
	$(V)$(MAKE) $(BUILD) TARGETS="$(OBJS)" CFLAGS="$(CFLAGS)"

dirs:
	$(V)$(MAKE) -C boot/

