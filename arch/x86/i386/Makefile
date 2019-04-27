OBJS += init.o
OBJS += cpu.o
OBJS += idt.o interrupt.o
OBJS += paging.o
OBJS += elf.o
OBJS += system-call.o
OBJS += spinlock.o
OBJS += task.o task-switch.o

CFLAGS += -I ../

ifdef AP_BIN
  AP_START += ap-start.o

  AP_CFLAGS += $(CFLAGS)
  AP_CFLAGS += -Wall -m32
  AP_LDFLAGS += -melf_i386 -Tap.ld

  AP_TARGET := ap
endif

.PHONY := all
all: compile $(AP_TARGET)

compile:
	$(V)$(MAKE) $(BUILD) TARGETS="$(OBJS)" CFLAGS="$(CFLAGS)"

ap:
	$(V)$(MAKE) $(BUILD_BIN) TARGETS="$(AP_START)" OUTPUT="$(AP_BIN)"	\
		CFLAGS="$(AP_CFLAGS)" LDFLAGS="$(AP_LDFLAGS)"

