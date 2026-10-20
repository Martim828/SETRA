# Exercise 3.2 - fork + waitpid
#
#   make run                    the parent finishes after the child
#   make run ARGS=--no-wait     the parent finishes first

include ../common/common.mk

TARGET = $(BUILD_DIR)/fork2

all: $(TARGET)

$(TARGET): fork2.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

run: $(TARGET)
	./$(TARGET) $(ARGS)
