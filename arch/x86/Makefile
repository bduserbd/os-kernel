OBJS += init.o
OBJS += cpu.o mp.o lapic.o
OBJS += idt.o interrupt.o
OBJS += smp.o ap.o
OBJS += video.o

CFLAGS += -I ../

AP_TRAMPOLINE += trampoline.o
LDFLAGS += -melf_i386 -Tap.ld

.PHONY := all
all: compile dirs

compile:
	$(V)$(MAKE) $(BUILD) TARGETS="$(OBJS)" CFLAGS="$(CFLAGS)"
	$(V)$(MAKE) $(BUILD_BIN) TARGETS="$(AP_TRAMPOLINE)" OUTPUT="$(AP_BIN)"	\
		CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)"

dirs:
	$(V)$(MAKE) -C boot/

