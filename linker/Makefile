ifeq ($(ARCH),x86)
  ifeq ($(SUB_X86_ARCH),i386)
    TARGET := grub-i386.ld.S
  else ifeq ($(SUB_X86_ARCH),x86_64)
    TARGET := grub-x86_64.ld.S
  endif
endif

ifndef TARGET
  $(error Unsupported architecture type)
endif

.PHONY: all
all: 
	$(V)$(CC) $(BUILD_CPPFLAGS) -E -x c $(TARGET) | grep -v '^#' > ../$(BUILD_LD_SCRIPT)

