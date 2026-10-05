# Exercise 2.2 - Swap with pointers / pointer-to-pointer

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
TARGET    = $(BUILD_DIR)/ex2_2$(EXE)

.PHONY: all run clean

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $@

$(TARGET): ex2_2.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)
