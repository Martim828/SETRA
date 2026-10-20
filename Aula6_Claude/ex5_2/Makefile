# Exercise 5.2 - threads sharing a counter (flag + polling, mutex)
#
#   make run                     counts to 5
#   make run N_ITERATIONS=10     rebuilds it to count to 10

include ../common/common.mk

N_ITERATIONS ?= 5
TARGET = $(BUILD_DIR)/shared_counter

all: $(TARGET)

# the value is saved in a file, so changing it rebuilds the program
$(BUILD_DIR)/n_iterations.$(N_ITERATIONS): | $(BUILD_DIR)
	rm -f $(BUILD_DIR)/n_iterations.*
	touch $@

$(TARGET): shared_counter.c $(BUILD_DIR)/n_iterations.$(N_ITERATIONS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -pthread -DN_ITERATIONS=$(N_ITERATIONS) $< -o $@

run: $(TARGET)
	./$(TARGET)
