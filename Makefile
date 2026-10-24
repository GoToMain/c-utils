#
#  Copyright (c) 2021-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
#
#  SPDX-License-Identifier: Apache-2.0
#

O       ?= .
NAME    ?= utils
SRC     := $(wildcard src/*.c)
OBJ     := $(SRC:%.c=%.o)
CCFLAGS ?= -Wall -Wextra -O3

ifeq ($(V),)
Q    := @
else
Q    :=
endif

.PHONY: all
all: $(O)/lib$(NAME).a
	@echo > /dev/null

$(O)/lib$(NAME).a: $(OBJ)
	@echo "  AR $(@F)"
	$(Q)$(AR) -qc $@ $^

$(O)/src/%.o: src/%.c
	@echo "  CC $<"
	@mkdir -p $(@D)
	$(Q)$(CC) -c $< $(CCFLAGS) -Iinclude/ -o $@

.PHONY: clean
clean:
	$(Q)rm -f $(OBJ) $(O)/lib$(NAME).a
