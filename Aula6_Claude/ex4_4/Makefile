# Exercise 4.4 - parent sends SIGUSR1 to the child with kill()
#
#   make run     takes about 10 seconds

include ../common/common.mk

TARGET = $(BUILD_DIR)/fork_signals

all: $(TARGET)

$(TARGET): fork_signals.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

run: $(TARGET)
	./$(TARGET)
