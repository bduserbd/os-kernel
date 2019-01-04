ACPICA := acpica

ACMYOS := acmyos.h

ACPICA_DIR := $(CURDIR)/$(ACPICA)
ACPICA_COMPONENTS_DIR := $(ACPICA)/source/components
ACPICA_INCLUDE_DIR := $(ACPICA)/source/include

ifeq (,$(wildcard $(ACPICA_DIR)))
  $(error ACPICA source code not found - you can get from https://acpica.org/downloads)
endif

ACPICA_SOURCES := $(wildcard $(ACPICA_COMPONENTS_DIR)/*/*.c)
ACPICA_SOURCES := $(filter-out $(ACPICA_COMPONENTS_DIR)/debugger/%,$(ACPICA_SOURCES))
ACPICA_SOURCES := $(filter-out $(ACPICA_COMPONENTS_DIR)/disassembler/%,$(ACPICA_SOURCES))

ACPICA_OBJS := $(ACPICA_SOURCES:%.c=%.o)

ACPICA_CFLAGS += -I $(ACPICA_INCLUDE_DIR)
ACPICA_CFLAGS += -D BUILDING_ACPICA

.PHONY := all
all:
	$(V)cp $(ACMYOS) $(ACPICA_INCLUDE_DIR)/platform
	$(V)$(MAKE) $(BUILD) TARGETS="$(ACPICA_OBJS)" CFLAGS="$(ACPICA_CFLAGS)"

