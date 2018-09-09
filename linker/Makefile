TARGETS := bios-grub-x86.ld.S

.PHONY: all
all: $(TARGETS)

%.ld.S:
	$(V)$(CC) $(BUILD_CPPFLAGS) -E -x c linker/$@ | grep -v '^#' > $(BUILD_LD_SCRIPT)

