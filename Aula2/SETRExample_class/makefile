# Paulo Pedreiras, Set/2026
# Adapted from ThrowTheSwitch.org

MODULE_NAME = MyVectorLib

# Paths
UNITY_ROOT = ../Unity
SRC_FOLDER = src
TEST_FOLDER = test
BUILD_FOLDER = build

# Commands
CLEANUP = rm -rf
MKDIR = mkdir -p

# Compiler & Flags
C_COMPILER = gcc
CFLAGS = -std=c89
CFLAGS += -Wall -Wextra
CFLAGS += -Wpointer-arith
CFLAGS += -Wcast-align
CFLAGS += -Wwrite-strings
CFLAGS += -Wswitch-default
CFLAGS += -Wunreachable-code
CFLAGS += -Winit-self
CFLAGS += -Wmissing-field-initializers
CFLAGS += -Wno-unknown-pragmas
CFLAGS += -Wstrict-prototypes
CFLAGS += -Wundef
CFLAGS += -Wold-style-definition

LDFLAGS = -lm
INC_DIRS = -I$(SRC_FOLDER) -I$(UNITY_ROOT)/src
SYMBOLS =

TARGET = $(BUILD_FOLDER)/test_$(MODULE_NAME)
SRC_FILES = $(UNITY_ROOT)/src/unity.c \
            $(SRC_FOLDER)/$(MODULE_NAME).c \
            $(TEST_FOLDER)/test$(MODULE_NAME).c

.PHONY: all default test clean

all: default

default: test

$(BUILD_FOLDER):
	$(MKDIR) $(BUILD_FOLDER)

$(TARGET): $(SRC_FILES) | $(BUILD_FOLDER)
	$(C_COMPILER) $(CFLAGS) $(INC_DIRS) $(SYMBOLS) $(SRC_FILES) -o $(TARGET) $(LDFLAGS)

test: $(TARGET)
	-./$(TARGET)

clean:
	$(CLEANUP) $(BUILD_FOLDER)
