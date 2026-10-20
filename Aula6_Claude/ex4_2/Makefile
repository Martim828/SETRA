# Exercise 4.2 - timer controlled by signals
#
#   make run     runs it in the foreground: CTRL+C resets it, and from
#                another terminal "kill -HUP <PID>" / "kill -CONT <PID>"
#                suspend and resume it; stop it with "kill <PID>"

include ../common/common.mk

TARGET = $(BUILD_DIR)/timer

all: $(TARGET)

$(TARGET): timer.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

run: $(TARGET)
	./$(TARGET)
