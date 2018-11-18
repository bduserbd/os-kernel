ifeq ($(FIRMWARE),BIOS)
  OBJS += multiboot.o
  OBJS += main-bios.o
else ifeq ($(FIRMWARE),UEFI)
  OBJS += multiboot2.o
  OBJS += main-uefi.o
endif

OBJS += start.o
OBJS += cpu.o paging.o

CFLAGS += -I ../

.PHONY: all
all:
	$(V)$(MAKE) $(BUILD) TARGETS="$(OBJS)" CFLAGS="$(CFLAGS)"

