# Exercise 2.2 - mywc with fork + exec
#
#   make run                     counts ../ex1_2/sample.txt
#   make run FILE=other.txt

include ../common/common.mk

TARGET = $(BUILD_DIR)/mywc_exec
FILE  ?= ../ex1_2/sample.txt

all: $(TARGET)

$(TARGET): mywc_exec.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

run: $(TARGET)
	./$(TARGET) "$(FILE)"
