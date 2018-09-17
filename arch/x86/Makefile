OBJS += init.o
OBJS += cpu.o mp.o lapic.o
OBJS += idt.o interrupt.o
OBJS += video.o

CFLAGS += -I ../

ifdef AP_BIN
  OBJS += smp.o ap.o
  AP_TRAMPOLINE += trampoline.o
  LDFLAGS += -melf_i386 -Tap.ld
  AP_TARGET := ap
endif

.PHONY := all
all: compile dirs $(AP_TARGET)

compile:
	$(V)$(MAKE) $(BUILD) TARGETS="$(OBJS)" CFLAGS="$(CFLAGS)"

dirs:
	$(V)$(MAKE) -C boot/

ap:
	$(V)$(MAKE) $(BUILD_BIN) TARGETS="$(AP_TRAMPOLINE)" OUTPUT="$(AP_BIN)"	\
		CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)"

