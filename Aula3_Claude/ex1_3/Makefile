# Exercise 1.3 - Interfaces / encapsulation (configData module + Unity tests)
#
#   make        builds the program and the unit tests
#   make run    runs the program
#   make test   runs the unit tests

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

UNITY_ROOT ?= ../Unity
SRC_DIR     = src
TEST_DIR    = test
BUILD_DIR   = build

APP  = $(BUILD_DIR)/ex1_3$(EXE)
TEST = $(BUILD_DIR)/test_configData$(EXE)

.PHONY: all run test clean

all: $(APP) $(TEST)

$(BUILD_DIR):
	mkdir -p $@

$(APP): $(SRC_DIR)/main.c $(SRC_DIR)/configData.c $(SRC_DIR)/configData.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(SRC_DIR) $(filter %.c,$^) -o $@

$(TEST): $(TEST_DIR)/test_configData.c $(SRC_DIR)/configData.c $(SRC_DIR)/configData.h \
         $(UNITY_ROOT)/src/unity.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(SRC_DIR) -I$(UNITY_ROOT)/src $(filter %.c,$^) -o $@

run: $(APP)
	./$(APP)

test: $(TEST)
	./$(TEST)

clean:
	rm -rf $(BUILD_DIR)
