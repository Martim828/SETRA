# Exercise 3.1 - fork
#
#   make run

include ../common/common.mk

TARGET = $(BUILD_DIR)/fork1

all: $(TARGET)

$(TARGET): fork1.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

run: $(TARGET)
	./$(TARGET)
