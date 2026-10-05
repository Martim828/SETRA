# Exercise 3.1 - Basic linked list (single file)

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
TARGET    = $(BUILD_DIR)/ex3_1$(EXE)

.PHONY: all run clean

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $@

$(TARGET): ex3_1.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)
