# Exercise 2.1 - myPS with exec
#
#   make run

include ../common/common.mk

TARGET = $(BUILD_DIR)/myPS_exec

all: $(TARGET)

$(TARGET): myPS_exec.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

run: $(TARGET)
	./$(TARGET)
