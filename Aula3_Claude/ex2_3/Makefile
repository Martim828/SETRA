# Exercise 2.3 - String reversal with pointer arithmetic
#
#   make run                        runs the checks
#   make run ARGS="'hello world'"   reverses the given text

CC      = gcc
CFLAGS  = -std=c99 -g
CFLAGS += -Wall -Wextra -Wpedantic -Wshadow
CFLAGS += -Wpointer-arith -Wcast-align -Wwrite-strings -Wswitch-default
CFLAGS += -Wunreachable-code -Winit-self -Wstrict-prototypes -Wundef
CFLAGS += -Wold-style-definition

# MinGW (Windows) adds .exe to the executables
ifeq ($(OS),Windows_NT)
EXE = .exe
endif

BUILD_DIR = build
TARGET    = $(BUILD_DIR)/ex2_3$(EXE)

.PHONY: all run clean

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $@

$(TARGET): ex2_3.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

run: $(TARGET)
	./$(TARGET) $(ARGS)

clean:
	rm -rf $(BUILD_DIR)
