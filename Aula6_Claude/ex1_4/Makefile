# Exercise 1.4 - deleteFold with system()
#
#   make run                creates Group1 ... Group5 inside build/ (one with a
#                           file inside) and removes them with deleteFold
#   make run ARGS="-i Group 5"   the same, asking before each one

include ../common/common.mk

TARGET = $(BUILD_DIR)/deleteFold
ARGS  ?= Group 5

all: $(TARGET)

$(TARGET): deleteFold.c $(COMMON_SRC) $(COMMON_HDR) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(COMMON_DIR) deleteFold.c $(COMMON_SRC) -o $@

run: $(TARGET)
	cd $(BUILD_DIR) && for i in 1 2 3 4 5; do mkdir -p Group$$i; done && \
		echo "not empty" > Group2/file.txt && ./deleteFold $(ARGS) && ls
