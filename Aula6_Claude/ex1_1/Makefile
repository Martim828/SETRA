# Exercise 1.1 - myPS with system()
#
#   make run

include ../common/common.mk

TARGET = $(BUILD_DIR)/myPS

all: $(TARGET)

$(TARGET): myPS.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

run: $(TARGET)
	./$(TARGET)
