# Exercise 2.3 - createFold and deleteFold with fork + exec
#
#   make run                     creates Group1 ... Group5 inside build/, puts a
#                                file in Group2 and removes them all again
#   make run ARGS="-i Group 5"   the same, asking before each removal

include ../common/common.mk

CREATE = $(BUILD_DIR)/createFold_exec
DELETE = $(BUILD_DIR)/deleteFold_exec
ARGS  ?= Group 5

all: $(CREATE) $(DELETE)

$(BUILD_DIR)/%: %.c $(COMMON_SRC) $(COMMON_HDR) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(COMMON_DIR) $< $(COMMON_SRC) -o $@

run: all
	cd $(BUILD_DIR) && ./createFold_exec Group 5 && ls && \
		echo "not empty" > Group2/file.txt && ./deleteFold_exec $(ARGS) && ls
