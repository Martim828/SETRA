# Theme 4 - Word Analyzer
#
#   make                          builds the program and the unit tests
#   make run                      analyzes data/sample.txt
#   make run FILE=other.txt N=5   analyzes other.txt, shows the 5 most frequent words
#   make test                     runs the unit tests
#   make clean                    removes the build folder
#
# Each .c file is compiled to its own .o, so after a change only the files
# that depend on it are rebuilt (the .d files list the headers each .c uses).

CC        = gcc
CFLAGS    = -std=c99 -g
CFLAGS   += -Wall -Wextra -Wpedantic -Wshadow
CFLAGS   += -Wpointer-arith -Wcast-align -Wwrite-strings -Wswitch-default
CFLAGS   += -Wunreachable-code -Winit-self -Wstrict-prototypes -Wundef
CFLAGS   += -Wold-style-definition
DEPFLAGS  = -MMD -MP

# MinGW (Windows) adds .exe to the executables
ifeq ($(OS),Windows_NT)
EXE = .exe
endif

UNITY_ROOT ?= ../Unity
SRC_DIR     = src
TEST_DIR    = test
BUILD_DIR   = build
CPPFLAGS    = -I$(SRC_DIR) -I$(UNITY_ROOT)/src

FILE ?= data/sample.txt
N    ?=

APP             = $(BUILD_DIR)/word_analyzer$(EXE)
TEST_WORD_LIST  = $(BUILD_DIR)/test_word_list$(EXE)
TEST_FILE_UTILS = $(BUILD_DIR)/test_file_utils$(EXE)

APP_OBJS   = $(BUILD_DIR)/main.o $(BUILD_DIR)/file_utils.o $(BUILD_DIR)/word_list.o
UNITY_OBJ  = $(BUILD_DIR)/unity.o

.PHONY: all run test clean

all: $(APP) $(TEST_WORD_LIST) $(TEST_FILE_UTILS)

# --- executables (link) ---------------------------------------------------

$(APP): $(APP_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(TEST_WORD_LIST): $(BUILD_DIR)/test_word_list.o $(BUILD_DIR)/word_list.o $(UNITY_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

$(TEST_FILE_UTILS): $(BUILD_DIR)/test_file_utils.o $(BUILD_DIR)/file_utils.o \
                    $(BUILD_DIR)/word_list.o $(UNITY_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

# --- object files (compile) -----------------------------------------------

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(DEPFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(DEPFLAGS) -c $< -o $@

$(UNITY_OBJ): $(UNITY_ROOT)/src/unity.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

# --- commands -------------------------------------------------------------

run: $(APP)
	./$(APP) $(FILE) $(N)

test: $(TEST_WORD_LIST) $(TEST_FILE_UTILS)
	./$(TEST_WORD_LIST)
	./$(TEST_FILE_UTILS)

clean:
	rm -rf $(BUILD_DIR)

-include $(wildcard $(BUILD_DIR)/*.d)
