# Aula 3 - Review of C, modular programming, Makefiles and unit tests
#
#   make          builds every exercise
#   make test     runs all the Unity unit tests (ex1_3, ex3_2, ex4_word_analyzer)
#   make check    runs every program (each one checks its own results) and the tests
#   make clean    removes every build folder
#
# Each exercise folder has its own Makefile and can also be built on its own.

EXERCISES  = ex1_1 ex1_2 ex1_3 ex2_1 ex2_2 ex2_3 ex3_1 ex3_2 ex4_word_analyzer
WITH_TESTS = ex1_3 ex3_2 ex4_word_analyzer

.PHONY: all test check clean $(EXERCISES)

all: $(EXERCISES)

$(EXERCISES):
	"$(MAKE)" -C $@

test: all
	@for dir in $(WITH_TESTS); do \
		"$(MAKE)" --no-print-directory -C $$dir test || exit 1; \
	done

# ex1_1 reads two numbers from stdin; ex4 shows only the 5 most frequent words
check: all
	echo "7 3" | "$(MAKE)" --no-print-directory -C ex1_1 run
	@for dir in $(filter-out ex1_1,$(EXERCISES)); do \
		"$(MAKE)" --no-print-directory -C $$dir run N=5 || exit 1; \
	done
	@"$(MAKE)" --no-print-directory test

clean:
	@for dir in $(EXERCISES); do \
		"$(MAKE)" --no-print-directory -C $$dir clean; \
	done
