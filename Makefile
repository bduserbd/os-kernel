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
BUILD_MODULE := -f $(CURDIR)/Makefile.module
BUILD_BIN := -f $(CURDIR)/Makefile.bin.build

BUILD_TOOLS := $(CURDIR)/tools

BUILD_OBJDIR := $(CURDIR)/objects
BUILD_MODULES_OBJDIR := $(BUILD_OBJDIR)/modules

BUILD_OBJS +=

BUILD_LD_SCRIPT = linker.ld

BUILD_CFLAGS += -Wall #-O2
BUILD_CFLAGS += -I $(CURDIR)
BUILD_CFLAGS += -Wno-main -nostdlib -fno-builtin -fno-strict-aliasing
BUILD_CFLAGS += $(BUILD_CPPFLAGS)
BUILD_CFLAGS += -g

BUILD_LDFLAGS += -T $(BUILD_LD_SCRIPT) -n

BUILD_OBJCOPYFLAGS += -O binary
BUILD_OBJCOPYFLAGS += -R .comment -R .eh_frame
BUILD_OBJCOPYFLAGS += -R .rel.eh_frame -R .note.GNU-stack

BUILD_CPPFLAGS +=

export BUILD BUILD_MODULE BUILD_BIN
export BUILD_OBJDIR BUILD_MODULES_OBJDIR BUILD_OBJS
export BUILD_LD_SCRIPT
export BUILD_CFLAGS BUILD_LDFLAGS BUILD_OBJCOPYFLAGS BUILD_CPPFLAGS

# Targets.
PHONY += all clean prep
PHONY += arch kernel modules
PHONY += link initramfs grub-iso

all: clean prep arch kernel modules link initramfs $(FIRMWARE_TARGET)

arch:
	$(V)$(MAKE) -C arch/$(ARCH)/

kernel:
	$(V)$(MAKE) -C kernel/

modules:
	$(V)$(MAKE) -C modules/

link:
	$(V)$(LD) $(BUILD_LDFLAGS) -o target.elf	\
		$(addprefix $(BUILD_OBJDIR)/,$(shell cat $(BUILD_OBJDIR)/objects.txt))
	$(V)if [ "$(ARCH)" = "x86" ] && [ "$(CPUS)" != "1" ]; then			\
		$(OBJCOPY) target.elf --update-section .ap_start=$(BUILD_OBJDIR)/$(AP_BIN);	\
	fi;
	$(OBJCOPY) target.elf --update-section .user=user-mode/init.o;	\

initramfs:
	$(V)$(patsubst $(CURDIR)/%,%,find $(BUILD_MODULES_OBJDIR))	\
		| cpio -o -H newc > real-initramfs.img
	$(V)$(BUILD_TOOLS)/initramfs.py real-initramfs.img initramfs.img

GRUB_BOOT_MENU = "					\n\
set timeout=0						\n\
set default=0						\n\
menuentry \"my os\" {					\n\
	multiboot /boot/kernel				\n\
	module /boot/initramfs.img initramfs.img	\n\
}"

bios-grub-iso:
	$(V)mkdir -p iso/boot/grub
	$(V)echo $(GRUB_BOOT_MENU) > iso/boot/grub/grub.cfg
	$(V)cp target.elf iso/boot/kernel
	$(V)cp initramfs.img iso/boot/initramfs.img
	$(V)grub-mkrescue -o target.iso iso/
	$(V)rm -rf iso/

GRUB_UEFI_BOOT_MENU = "					\n\
insmod efi_gop						\n\
set timeout=0						\n\
set default=0						\n\
menuentry \"my os\" {					\n\
	multiboot2 /boot/kernel				\n\
	module2 /boot/initramfs.img initramfs.img	\n\
}"

uefi-grub:
	$(V)mkdir -p boot/grub/$(SUB_X86_ARCH)-efi
	$(V)echo $(GRUB_UEFI_BOOT_MENU) > boot/grub/grub.cfg
	$(V)cp target.elf boot/kernel
	$(V)cp initramfs.img boot/initramfs.img
	$(V)cp /usr/lib/grub/$(SUB_X86_ARCH)-efi/* boot/grub/$(SUB_X86_ARCH)-efi
	$(V)grub-mkstandalone -O $(SUB_X86_ARCH)-efi -o $(UEFI_IMAGE_NAME) boot/

prep:
	$(V)$(MAKE) -C linker/
	$(V)mkdir -p $(BUILD_OBJDIR)
	$(V)mkdir -p $(BUILD_MODULES_OBJDIR)
	$(V)touch $(BUILD_OBJDIR)/objects.txt

clean:
	$(V)rm -f *.ld
	$(V)rm -f *.elf
	$(V)rm -f *.iso
	$(V)rm -rf $(BUILD_OBJDIR)

.PHONY: $(PHONY)

