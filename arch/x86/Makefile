OBJS += init.o
OBJS += cpu.o io.o
OBJS += idt.o 8259a.o lapic.o ioapic.o
OBJS += 8253.o tsc.o hpet.o
OBJS += time.o
#OBJS += system-call.o
#OBJS += gdt.o
OBJS += smbios.o
#OBJS += user-mode.o
OBJS += amd.o
OBJS += bits.o

ifeq ($(FIRMWARE),BIOS)
  OBJS += mp.o
  OBJS += video.o
endif

CFLAGS += -I ../

ifdef AP_BIN
  OBJS += smp.o ap.o
endif

.PHONY := all
all: compile dirs $(AP_TARGET)

compile:
	$(V)$(MAKE) $(BUILD) TARGETS="$(OBJS)" CFLAGS="$(CFLAGS)"

dirs:
	$(V)$(MAKE) -C $(SUB_X86_ARCH)/
	$(V)$(MAKE) -C boot/

