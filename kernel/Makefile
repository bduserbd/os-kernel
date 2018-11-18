OBJS += init.o

CFLAGS = -I ./

.PHONY := all
all: compile dirs

compile:
	$(V)$(MAKE) $(BUILD) TARGETS="$(OBJS)" CFLAGS="$(CFLAGS)"

dirs:
	$(V)$(MAKE) -C utils/
	$(V)$(MAKE) -C pci/
	$(V)$(MAKE) -C irq/
	$(V)$(MAKE) -C acpi/
	$(V)$(MAKE) -C fb/
	$(V)$(MAKE) -C mm/
	$(V)$(MAKE) -C video/
	$(V)$(MAKE) -C network/
	$(V)$(MAKE) -C modules/
	$(V)$(MAKE) -C initramfs/

