# Exercise 1.3 - createFold with system()
#
#   make run                     creates Group1 ... Group10 inside build/
#   make run ARGS="Test 3"

include ../common/common.mk

TARGET = $(BUILD_DIR)/createFold
ARGS  ?= Group 10

all: $(TARGET)

$(TARGET): createFold.c $(COMMON_SRC) $(COMMON_HDR) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(COMMON_DIR) createFold.c $(COMMON_SRC) -o $@

run: $(TARGET)
	cd $(BUILD_DIR) && ./createFold $(ARGS) && ls
