OBJS += cpu.o
OBJS += init.o
OBJS += paging.o

CFLAGS += -I ../

.PHONY := all
all:
	$(V)$(MAKE) $(BUILD) TARGETS="$(OBJS)" CFLAGS="$(CFLAGS)"

