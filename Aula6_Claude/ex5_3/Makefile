# Exercise 5.3 - signals sent to threads with pthread_kill
#
#   make run

include ../common/common.mk

TARGET = $(BUILD_DIR)/thread_signals

all: $(TARGET)

$(TARGET): thread_signals.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -pthread $< -o $@

run: $(TARGET)
	./$(TARGET)
