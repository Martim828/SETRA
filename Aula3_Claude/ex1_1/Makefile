# Exercise 1.1 - Keeping local (MyRand + cal module)

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
TARGET    = $(BUILD_DIR)/ex1$(EXE)

.PHONY: all run clean

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $@

$(TARGET): ex1.c cal.c ex1.h cal.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(filter %.c,$^) -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)
