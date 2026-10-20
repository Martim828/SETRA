# Settings shared by every exercise Makefile ("include ../common/common.mk").
# These programs use fork(), exec(), signals and POSIX threads, so they must be
# built and run in Linux (or WSL).

CC      = gcc
CFLAGS  = -std=gnu11 -g -D_GNU_SOURCE
CFLAGS += -Wall -Wextra -Wpedantic -Wshadow -Wwrite-strings
CFLAGS += -Wstrict-prototypes -Wold-style-definition -Wundef

BUILD_DIR  = build
COMMON_DIR = ../common
COMMON_SRC = $(COMMON_DIR)/utils.c
COMMON_HDR = $(COMMON_DIR)/utils.h

.DEFAULT_GOAL := all
.PHONY: all run clean

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)
