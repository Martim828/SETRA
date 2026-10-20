# Exercise 5.1 - two threads and pthread_join
#
#   make run

include ../common/common.mk

TARGET = $(BUILD_DIR)/threads

all: $(TARGET)

$(TARGET): threads.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -pthread $< -o $@

run: $(TARGET)
	./$(TARGET)
