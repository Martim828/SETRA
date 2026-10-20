# Aula 6 - Linux processes, signals and threads
#
#   make          builds every exercise
#   make check    runs every program and checks its output (about 1 minute)
#   make clean    removes every build folder
#
# Each exercise folder has its own Makefile and can also be built on its own
# ("make -C ex4_2 run"). Everything needs Linux or WSL.

EXERCISES = ex1_1 ex1_2 ex1_3 ex1_4 ex2_1 ex2_2 ex2_3 ex3_1 ex3_2 \
            ex4_1 ex4_2 ex4_3 ex4_4 ex4_5 ex5_1 ex5_2 ex5_3

.PHONY: all check clean $(EXERCISES)

all: $(EXERCISES)

$(EXERCISES):
	"$(MAKE)" --no-print-directory -C $@

check: all
	sh ./check.sh

clean:
	@for dir in $(EXERCISES); do \
		"$(MAKE)" --no-print-directory -C $$dir clean; \
	done
