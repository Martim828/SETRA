# Exercise 4.5 - blocking SIGUSR2 inside the SIGUSR1 handler with sa_mask
#
#   make run                  starts it in the background, sends SIGUSR1 and,
#                             one second later, SIGUSR2, then stops it
#   make run ARGS=--no-mask   the same without the mask, to compare

include ../common/common.mk

TARGET = $(BUILD_DIR)/sigmask

all: $(TARGET)

$(TARGET): sigmask.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

run: $(TARGET)
	@./$(TARGET) $(ARGS) & pid=$$!; sleep 0.5; \
	echo "kill -USR1 $$pid"; kill -USR1 $$pid; sleep 1; \
	echo "kill -USR2 $$pid"; kill -USR2 $$pid; sleep 3; \
	kill $$pid; wait $$pid 2>/dev/null; true
