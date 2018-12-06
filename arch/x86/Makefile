OBJS += init.o
OBJS += cpu.o io.o lapic.o ioapic.o
OBJS += idt.o interrupt.o 8259a.o 8253.o hpet.o
OBJS += system-call.o
OBJS += gdt.o
OBJS += smbios.o
OBJS += elf.o
OBJS += task.o task-switch.o
OBJS += user-mode.o
OBJS += log2.o

ifeq ($(FIRMWARE),BIOS)
  OBJS += mp.o
  OBJS += video.o
endif

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

