OBJS += cpu.o
OBJS += paging.o
OBJS += init.o

CFLAGS += -I ../

.PHONY := all
all:
	$(V)$(MAKE) $(BUILD) TARGETS="$(OBJS)" CFLAGS="$(CFLAGS)"

