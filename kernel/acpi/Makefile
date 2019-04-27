OBJS += acpi.o
OBJS += acpica.o acpica-osl.o

CFLAGS = -I ../

.PHONY: all
all:
	$(V)$(MAKE) $(BUILD) TARGETS="$(OBJS)" CFLAGS="$(CFLAGS)"

