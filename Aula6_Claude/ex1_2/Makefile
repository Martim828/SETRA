# Exercise 1.2 - mywc with system()
#
#   make run                     counts sample.txt
#   make run FILE=other.txt

include ../common/common.mk

TARGET = $(BUILD_DIR)/mywc
FILE  ?= sample.txt

all: $(TARGET)

$(TARGET): mywc.c $(COMMON_SRC) $(COMMON_HDR) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(COMMON_DIR) mywc.c $(COMMON_SRC) -o $@

run: $(TARGET)
	./$(TARGET) "$(FILE)"
