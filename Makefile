# Makefile for OS.

# Verbosity.
V = @
ifeq ("$(origin VERBOSE)", "command line")
ifneq ($(VERBOSE), 0)
  undefine V
endif
endif

export V

# Flags.
MAKEFLAGS += --no-print-directory

# Architecture.
SYSTEM_ARCH = $(shell uname -p | sed -e s/i.86/x86/ -e s/x86_64/x86/)
ARCH ?= $(SYSTEM_ARCH)

# Include configuration.
include Makefile.config

# Tools.
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
AS = $(CROSS_COMPILE)as
OBJCOPY = $(CROSS_COMPILE)objcopy

export CC LD AS OBJCOPY

# Common build options.
BUILD := -f $(CURDIR)/Makefile.build
BUILD_BIN := -f $(CURDIR)/Makefile.bin.build

BUILD_OBJDIR := $(CURDIR)/objects

BUILD_OBJS +=

BUILD_LD_SCRIPT = linker.ld

BUILD_CFLAGS += -Wall -m32 -O2
BUILD_CFLAGS += -I $(CURDIR)
BUILD_CFLAGS += -Wno-main -nostdlib -fno-builtin -fno-strict-aliasing
BUILD_CFLAGS += $(BUILD_CPPFLAGS)

BUILD_LDFLAGS += -T $(BUILD_LD_SCRIPT) -melf_i386 -n

BUILD_OBJCOPYFLAGS += -O binary
BUILD_OBJCOPYFLAGS += -R .comment -R .eh_frame
BUILD_OBJCOPYFLAGS += -R .rel.eh_frame -R .note.GNU-stack

BUILD_CPPFLAGS +=

export BUILD BUILD_BIN
export BUILD_OBJDIR BUILD_OBJS
export BUILD_LD_SCRIPT
export BUILD_CFLAGS BUILD_LDFLAGS BUILD_OBJCOPYFLAGS BUILD_CPPFLAGS

# Targets.
PHONY += all clean prep
PHONY += arch kernel
PHONY += link grub-iso

all: clean prep arch kernel link grub-iso

arch:
	$(V)$(MAKE) -C arch/$(ARCH)/

kernel:
	$(V)$(MAKE) -C kernel/

link:
	$(V)$(LD) $(BUILD_LDFLAGS) -o target.elf	\
		$(addprefix $(BUILD_OBJDIR)/,$(shell cat $(BUILD_OBJDIR)/objects.txt))
	$(V)if [ "$(ARCH)" = "x86" ] && [ "$(K_CONFIG_CPUS)" != 1 ]; then			\
		$(OBJCOPY) target.elf --update-section .ap_start=$(BUILD_OBJDIR)/$(AP_BIN);	\
	fi;

GRUB_BOOT_MENU = "			\n\
set timeout=0				\n\
set default=0				\n\
menuentry \"my os\" {			\n\
	multiboot2 /boot/kernel		\n\
}"

grub-iso:
	$(V)mkdir -p iso/boot/grub
	$(V)echo $(GRUB_BOOT_MENU) > grub.cfg
	$(V)cp grub.cfg iso/boot/grub
	$(V)cp target.elf iso/boot/kernel
	$(V)grub-mkrescue -o target.iso iso
	$(V)rm grub.cfg
	$(V)rm -rf iso/

prep:
	$(V)$(MAKE) -C linker/
	$(V)mkdir -p $(BUILD_OBJDIR)
	$(V)touch $(BUILD_OBJDIR)/objects.txt

clean:
	$(V)rm -f *.ld
	$(V)rm -f *.elf
	$(V)rm -f *.iso
	$(V)rm -rf $(BUILD_OBJDIR)

.PHONY: $(PHONY)

