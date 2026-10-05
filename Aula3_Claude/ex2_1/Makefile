# Exercise 2.1 - Dynamic array statistics
#
#   make run                    N = 10, M = 100, random seed
#   make run ARGS="20 6 1234"   N = 20, M = 6, seed = 1234

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
TARGET    = $(BUILD_DIR)/ex2_1$(EXE)

.PHONY: all run clean

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $@

$(TARGET): ex2_1.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

run: $(TARGET)
	./$(TARGET) $(ARGS)

clean:
	rm -rf $(BUILD_DIR)
