# Exercise 4.1 - catching signals with sigaction
#
#   make run     runs it in the foreground: try CTRL+C, and from another
#                terminal "kill -USR1 <PID>", "kill -HUP <PID>", ...
#                stop it with "kill <PID>"

include ../common/common.mk

TARGET = $(BUILD_DIR)/catch_signals

all: $(TARGET)

$(TARGET): catch_signals.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

run: $(TARGET)
	./$(TARGET)
