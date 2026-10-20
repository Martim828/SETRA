# Exercise 4.3 - timer that ignores every other signal
#
#   make run     as in 4.2, but "kill -USR1 <PID>", "kill <PID>" and others
#                are ignored: stop it with "kill -KILL <PID>"

include ../common/common.mk

TARGET = $(BUILD_DIR)/timer_ignore

all: $(TARGET)

$(TARGET): timer_ignore.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

run: $(TARGET)
	./$(TARGET)
