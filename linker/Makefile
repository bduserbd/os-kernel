TARGET = bios-grub-x86.ld.S

.PHONY: all
all: 
	$(V)$(CC) $(BUILD_CPPFLAGS) -E -x c $(TARGET) | grep -v '^#' > ../$(BUILD_LD_SCRIPT)

