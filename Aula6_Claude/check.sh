#!/bin/sh
# Runs every exercise and checks its output ("make check" builds them first).
# The signal exercises are started in the background and the signals are sent
# with kill, as the guide suggests. Takes about one minute.

cd "$(dirname "$0")" || exit 1
ROOT=$(pwd)
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT

passed=0
failed=0

ok() {
    passed=$((passed + 1))
    echo "  ok    $1"
}

fail() {
    failed=$((failed + 1))
    echo "  FAIL  $1"
    if [ -n "$2" ] && [ -f "$2" ]; then
        echo "        --- output ---"
        sed 's/^/        /' "$2"
        echo "        --------------"
    fi
}

# check <description> <condition...>: ok if the command succeeds
check() {
    desc=$1
    shift
    if "$@"; then ok "$desc"; else fail "$desc" "$OUT"; fi
}

has() { grep -Eq -- "$1" "$OUT"; }
count() { grep -Ec -- "$1" "$OUT"; }
line_of() { grep -En -- "$1" "$OUT" | head -n 1 | cut -d: -f1; }

# before <regex1> <regex2>: the first match of regex1 comes before that of regex2
before() {
    a=$(line_of "$1")
    b=$(line_of "$2")
    [ -n "$a" ] && [ -n "$b" ] && [ "$a" -lt "$b" ]
}

alive() { kill -0 "$1" 2>/dev/null; }

# start_bg <program> [args]: runs it in the background with output in $OUT
start_bg() {
    "$@" > "$OUT" 2>&1 &
    PID=$!
    sleep 0.5
}

stop_bg() {
    kill -KILL "$PID" 2>/dev/null
    wait "$PID" 2>/dev/null
}

# ---------------------------------------------------------------- part 1 & 2
echo "Part 1 and 2: system() and exec"

for prog in ex1_1/build/myPS ex2_1/build/myPS_exec; do
    OUT=$TMP/ps.out
    "$prog" > "$OUT"
    check "$prog prints the ps aux table" has '^USER +PID +%CPU +%MEM'
done

# a name with spaces, a quote, $ and ; must not break the shell command
WEIRD="$TMP/it's a \$file; touch HACKED"
cp ex1_2/sample.txt "$WEIRD"
for file in ex1_2/sample.txt "$WEIRD"; do
    chars=$(wc -m < "$file" | tr -d ' ')
    words=$(wc -w < "$file" | tr -d ' ')
    for prog in ex1_2/build/mywc ex2_2/build/mywc_exec; do
        OUT=$TMP/wc.out
        "$prog" "$file" > "$OUT" 2>&1
        check "$prog $(basename "$file"): $chars characters, $words words" \
            has "^Characters: $chars\$"
        check "$prog $(basename "$file"): words" has "^Words: $words\$"
    done
done
check "no command was injected through the file name" [ ! -e "$TMP/HACKED" ]
OUT=$TMP/wc.out
ex1_2/build/mywc "$TMP/missing.txt" > "$OUT" 2>&1
check "mywc fails on a missing file" [ $? -ne 0 ]

for pair in "ex1_3/build/createFold ex1_4/build/deleteFold" \
            "ex2_3/build/createFold_exec ex2_3/build/deleteFold_exec"; do
    set -- $pair
    create=$ROOT/$1
    delete=$ROOT/$2
    DIR=$TMP/folders
    rm -rf "$DIR"
    mkdir "$DIR"
    OUT=$TMP/folders.out

    (cd "$DIR" && "$create" Group 10) > "$OUT"
    check "$(basename "$create") Group 10 creates Group1..Group10" \
        [ "$(cd "$DIR" && ls -d Group* | wc -l)" -eq 10 -a -d "$DIR/Group10" ]

    (cd "$DIR" && "$create" "x'; touch PWNED; '" 2) > "$OUT"
    check "$(basename "$create") with a quote in the name" \
        [ -d "$DIR/x'; touch PWNED; '2" -a ! -e "$DIR/PWNED" ]

    for bad in 0 100 abc 5x; do
        (cd "$DIR" && "$create" Bad "$bad") > "$OUT" 2>&1
        check "$(basename "$create") rejects n = $bad" [ $? -ne 0 -a ! -e "$DIR/Bad1" ]
    done

    echo data > "$DIR/Group3/file.txt"
    (cd "$DIR" && "$delete" Group 10) > "$OUT"
    check "$(basename "$delete") Group 10 removes them all (Group3 not empty)" \
        [ -z "$(cd "$DIR" && ls -d Group* 2>/dev/null)" ]

    (cd "$DIR" && "$create" Group 3 && printf 'y\nn\ny\n' | "$delete" -i Group 4) > "$OUT"
    check "$(basename "$delete") -i: answers y n y keep only Group2" \
        [ "$(cd "$DIR" && ls -d Group*)" = "Group2" ]
    check "$(basename "$delete") -i: Group4 does not exist" has '^Skipping Group4'
done

# -------------------------------------------------------------------- part 3
echo "Part 3: fork"

OUT=$TMP/fork1.out
ex3_1/build/fork1 > "$OUT"
parent=$(sed -n 's/^Parent: my PID is \([0-9]*\).*/\1/p' "$OUT")
child=$(sed -n "s/^Parent: my PID is [0-9]*, my child's PID is \([0-9]*\)/\1/p" "$OUT")
check "fork1: the parent reports the child's PID" has "^Child: +my PID is $child,"
check "fork1: both processes say they are finishing" \
    [ "$(count 'is finishing now$')" -eq 2 ]
check "fork1: the message before fork() is printed once" \
    [ "$(count 'is going to fork')" -eq 1 ]

OUT=$TMP/fork2.out
ex3_2/build/fork2 > "$OUT"
parent=$(sed -n 's/.*Parent (PID \([0-9]*\)) starting/\1/p' "$OUT")
child=$(sed -n 's/.*created child PID \([0-9]*\)/\1/p' "$OUT")
check "fork2: the parent finishes after the child" \
    before "Process PID $child is finishing" "Process PID $parent is finishing"
check "fork2: the last line is the parent's" \
    sh -c "tail -n 1 '$OUT' | grep -q 'Process PID $parent is finishing'"
check "fork2: the parent gets the child's exit status" has "child $child exited with status 0"

OUT=$TMP/fork2_nowait.out
ex3_2/build/fork2 --no-wait > "$OUT"
sleep 3.5 # the orphan child keeps writing to the file
parent=$(sed -n 's/.*Parent (PID \([0-9]*\)) starting/\1/p' "$OUT")
child=$(sed -n 's/.*created child PID \([0-9]*\)/\1/p' "$OUT")
check "fork2 --no-wait: the parent finishes first" \
    before "Process PID $parent is finishing" "Process PID $child is finishing"
check "fork2 --no-wait: the orphan child gets a new parent" \
    sh -c "grep 'Child working... 3/3' '$OUT' | grep -vq 'parent is now PID $parent)'"

# -------------------------------------------------------------------- part 4
echo "Part 4: signals"

OUT=$TMP/sig41.out
start_bg ex4_1/build/catch_signals
for sig in USR1 HUP CONT INT; do
    kill -$sig "$PID"
    sleep 0.3
done
check "catch_signals: SIGUSR1, SIGHUP, SIGCONT, SIGINT in order" \
    sh -c "grep '^Received' '$OUT' | tr '\n' ' ' | grep -q 'SIGUSR1 Received SIGHUP Received SIGCONT Received SIGINT'"
check "catch_signals: still running after SIGINT" alive "$PID"
kill -TERM "$PID"
sleep 0.3
check "catch_signals: SIGTERM (not caught) terminates it" sh -c "! kill -0 $PID 2>/dev/null"
wait "$PID" 2>/dev/null

for prog in ex4_2/build/timer ex4_3/build/timer_ignore; do
    name=$(basename "$prog")
    OUT=$TMP/$name.out
    start_bg "$prog"
    sleep 1.6
    check "$name: counts the seconds" has '^1 s$'

    kill -HUP "$PID"
    sleep 0.5
    at=$(sed -n 's/^SIGHUP: counting suspended at \([0-9]*\) s$/\1/p' "$OUT")
    check "$name: SIGHUP suspends it" [ -n "$at" ]
    lines=$(wc -l < "$OUT")
    sleep 2
    check "$name: does not count while suspended" [ "$(wc -l < "$OUT")" -eq "$lines" ]

    kill -CONT "$PID"
    sleep 1.3
    check "$name: SIGCONT resumes it where it stopped" has "^SIGCONT: counting resumed at $at s\$"
    check "$name: counts again after SIGCONT" has "^$((at + 1)) s\$"

    kill -INT "$PID"
    sleep 1.3
    check "$name: SIGINT resets it to 0 and keeps counting" \
        [ "$(sed -n '/^SIGINT: reset/{n;p;q;}' "$OUT")" = "1 s" ]

    if [ "$name" = timer ]; then
        kill -USR1 "$PID"
        sleep 0.3
        check "timer: an uncaught signal (SIGUSR1) terminates it" sh -c "! kill -0 $PID 2>/dev/null"
    else
        for sig in USR1 USR2 TERM QUIT ALRM PIPE TSTP TTIN WINCH; do
            kill -$sig "$PID"
        done
        lines=$(wc -l < "$OUT")
        sleep 1.3
        check "timer_ignore: ignores USR1 USR2 TERM QUIT ALRM PIPE TSTP TTIN WINCH" alive "$PID"
        check "timer_ignore: keeps counting after them" [ "$(wc -l < "$OUT")" -gt "$lines" ]
    fi
    stop_bg
done

OUT=$TMP/sig44.out
timeout 20 ex4_4/build/fork_signals > "$OUT" 2>&1
check "fork_signals: the parent sends 10 signals" [ "$(count 'Parent .*sending SIGUSR1')" -eq 10 ]
check "fork_signals: the child receives all 10" [ "$(count 'Child .*received SIGUSR1 #')" -eq 10 ]
check "fork_signals: the child finishes after #10" before 'received SIGUSR1 #10' 'got all 10 signals'
check "fork_signals: the parent finishes last" \
    sh -c "tail -n 1 '$OUT' | grep -q 'Parent .*child finished'"

for mode in mask no-mask; do
    OUT=$TMP/sig45_$mode.out
    if [ $mode = mask ]; then start_bg ex4_5/build/sigmask; else start_bg ex4_5/build/sigmask --no-mask; fi
    kill -USR1 "$PID"
    sleep 1
    kill -USR2 "$PID"
    sleep 3
    if [ $mode = mask ]; then
        check "sigmask: SIGUSR2 waits until the SIGUSR1 handler ends" \
            before '^SIGUSR1 handler: end' '^SIGUSR2 handler'
        check "sigmask: SIGUSR2 is still delivered afterwards" has '^SIGUSR2 handler: runs now \(counter = 1\)'
    else
        check "sigmask --no-mask: SIGUSR2 interrupts the SIGUSR1 handler" \
            before '^SIGUSR2 handler' '^SIGUSR1 handler: end'
    fi
    stop_bg
done

# -------------------------------------------------------------------- part 5
echo "Part 5: POSIX threads"

OUT=$TMP/threads.out
timeout 10 ex5_1/build/threads > "$OUT"
check "threads: both threads start" [ "$(count '^Thread [12] starting$')" -eq 2 ]
check "threads: thread 1 (2 s) finishes before thread 2 (3 s)" \
    before '^Thread 1 finishing' '^Thread 2 finishing'
check "threads: the main thread finishes last" \
    sh -c "tail -n 1 '$OUT' | grep -q '^Main thread: both threads finished, exiting.$'"

OUT=$TMP/counter.out
timeout 15 ex5_2/build/shared_counter > "$OUT"
check "shared_counter: prints 1, 2, 3, 4, 5 once each" \
    [ "$(grep '^Printer' "$OUT" | tr -dc '0-9')" = 12345 ]
gcc -std=gnu11 -pthread -DN_ITERATIONS=2 ex5_2/shared_counter.c -o "$TMP/counter2"
"$TMP/counter2" > "$OUT"
check "shared_counter: N_ITERATIONS=2 at compile time stops at 2" \
    [ "$(grep '^Printer' "$OUT" | tr -dc '0-9')" = 12 ]

OUT=$TMP/thread_signals.out
timeout 10 ex5_3/build/thread_signals > "$OUT"
check "thread_signals: does not hang" [ $? -eq 0 ]
check "thread_signals: thread 1 terminates on the first SIGUSR1" \
    before '^Thread 1: received signal 10' 'sending SIGUSR1 to thread 2'
check "thread_signals: thread 2 terminates on the second" has '^Thread 2: received signal 10'
check "thread_signals: the main thread finishes last" \
    sh -c "tail -n 1 '$OUT' | grep -q '^Main: both threads terminated'"

echo
echo "$passed passed, $failed failed"
[ "$failed" -eq 0 ]
