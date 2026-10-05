/* Unity tests for the word_list module (word analyzer) */
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "word_list.h"

static WordList *list;

/* Every test starts with a new, empty list */
void setUp(void) {
    list = word_list_create();
    TEST_ASSERT_NOT_NULL(list);
}

void tearDown(void) {
    word_list_destroy(list);
    list = NULL;
}

/* ---------------------------------------------------------------------- */
/* Helpers                                                                 */
/* ---------------------------------------------------------------------- */

static void add_times(const char *word, int times) {
    for (int i = 0; i < times; i++) {
        TEST_ASSERT_EQUAL_INT(WORD_LIST_OK, word_list_add(list, word));
    }
}

/* Checks the entry at position index */
static void assert_entry(size_t index, const char *word, unsigned long count) {
    const char *w = NULL;
    unsigned long c = 0;

    TEST_ASSERT_EQUAL_INT(WORD_LIST_OK, word_list_get(list, index, &w, &c));
    TEST_ASSERT_EQUAL_STRING(word, w);
    TEST_ASSERT_EQUAL_UINT(count, c);
}

/* tmpfile() is standard C, but with MinGW it needs administrator rights (it
 * creates the file in C:\), so in that case a file in build/ is used */
#define SCRATCH_NAME "build/test_word_list.tmp"

static FILE *scratch_open(void) {
    FILE *f = tmpfile();
    return f != NULL ? f : fopen(SCRATCH_NAME, "w+b");
}

static void scratch_close(FILE *f) {
    fclose(f);
    remove(SCRATCH_NAME);       /* nothing to remove if tmpfile() worked */
}

/* Prints the list into a temporary file and reads the text back */
static void print_to_string(size_t max_entries, char *buffer, size_t size) {
    FILE *f = scratch_open();
    size_t n;

    if (f == NULL) {
        TEST_IGNORE_MESSAGE("cannot create a temporary file");
        return;
    }
    word_list_print(list, f, max_entries);
    rewind(f);
    n = fread(buffer, 1, size - 1, f);
    buffer[n] = '\0';
    scratch_close(f);
}

/* ---------------------------------------------------------------------- */
/* Tests                                                                   */
/* ---------------------------------------------------------------------- */

void test_Create_IsEmpty(void) {
    TEST_ASSERT_EQUAL_size_t(0, word_list_size(list));
    TEST_ASSERT_EQUAL_UINT(0, word_list_total(list));
    TEST_ASSERT_EQUAL_UINT(0, word_list_frequency(list, "word"));
    TEST_ASSERT_EQUAL_INT(WORD_LIST_NOT_FOUND, word_list_get(list, 0, NULL, NULL));
}

void test_Add_NewWordHasCountOne(void) {
    add_times("hello", 1);
    TEST_ASSERT_EQUAL_size_t(1, word_list_size(list));
    TEST_ASSERT_EQUAL_UINT(1, word_list_frequency(list, "hello"));
    assert_entry(0, "hello", 1);
}

void test_Add_SameWordIncrementsCount(void) {
    add_times("again", 3);
    TEST_ASSERT_EQUAL_size_t(1, word_list_size(list));
    TEST_ASSERT_EQUAL_UINT(3, word_list_total(list));
    TEST_ASSERT_EQUAL_UINT(3, word_list_frequency(list, "again"));
}

void test_Add_KeepsOrderOfFirstAppearance(void) {
    add_times("b", 1);
    add_times("a", 1);
    add_times("c", 1);
    add_times("a", 1);

    TEST_ASSERT_EQUAL_size_t(3, word_list_size(list));
    TEST_ASSERT_EQUAL_UINT(4, word_list_total(list));
    assert_entry(0, "b", 1);
    assert_entry(1, "a", 2);
    assert_entry(2, "c", 1);
}

void test_Add_StoresACopyOfTheWord(void) {
    char buffer[] = "copy";

    add_times(buffer, 1);
    buffer[0] = 'X';            /* changing the caller's buffer must not matter */
    TEST_ASSERT_EQUAL_UINT(1, word_list_frequency(list, "copy"));
    TEST_ASSERT_EQUAL_UINT(0, word_list_frequency(list, "Xopy"));
}

void test_Add_InvalidArguments(void) {
    TEST_ASSERT_EQUAL_INT(WORD_LIST_ERR_ARG, word_list_add(NULL, "word"));
    TEST_ASSERT_EQUAL_INT(WORD_LIST_ERR_ARG, word_list_add(list, NULL));
    TEST_ASSERT_EQUAL_INT(WORD_LIST_ERR_ARG, word_list_add(list, ""));
    TEST_ASSERT_EQUAL_size_t(0, word_list_size(list));
}

void test_Frequency_IsCaseSensitive(void) {
    add_times("Word", 1);
    TEST_ASSERT_EQUAL_UINT(1, word_list_frequency(list, "Word"));
    TEST_ASSERT_EQUAL_UINT(0, word_list_frequency(list, "word"));
}

void test_Sort_ByDecreasingFrequency(void) {
    add_times("one", 1);
    add_times("three", 3);
    add_times("two", 2);

    word_list_sort_by_frequency(list);
    assert_entry(0, "three", 3);
    assert_entry(1, "two", 2);
    assert_entry(2, "one", 1);
}

void test_Sort_TiesInAlphabeticalOrder(void) {
    add_times("pear", 2);
    add_times("kiwi", 5);
    add_times("apple", 2);
    add_times("fig", 2);

    word_list_sort_by_frequency(list);
    assert_entry(0, "kiwi", 5);
    assert_entry(1, "apple", 2);
    assert_entry(2, "fig", 2);
    assert_entry(3, "pear", 2);
}

void test_Sort_EmptyAndSingleWord(void) {
    word_list_sort_by_frequency(list);
    TEST_ASSERT_EQUAL_size_t(0, word_list_size(list));

    add_times("only", 2);
    word_list_sort_by_frequency(list);
    assert_entry(0, "only", 2);

    word_list_sort_by_frequency(NULL);      /* must not crash */
}

void test_Sort_ManyWords(void) {
    char word[16];
    unsigned long total = 0;
    unsigned long previous_count = ULONG_MAX;
    const char *previous_word = "";

    for (int i = 0; i < 200; i++) {
        snprintf(word, sizeof word, "w%03d", (199 * i) % 200);    /* shuffled */
        add_times(word, i % 7 + 1);
        total += (unsigned long)(i % 7 + 1);
    }
    word_list_sort_by_frequency(list);

    TEST_ASSERT_EQUAL_size_t(200, word_list_size(list));
    TEST_ASSERT_EQUAL_UINT(total, word_list_total(list));
    for (size_t i = 0; i < 200; i++) {
        const char *w;
        unsigned long c;

        TEST_ASSERT_EQUAL_INT(WORD_LIST_OK, word_list_get(list, i, &w, &c));
        TEST_ASSERT_TRUE(c <= previous_count);                 /* never increases */
        if (c == previous_count) {
            TEST_ASSERT_TRUE(strcmp(previous_word, w) < 0);    /* ties: a..z */
        }
        previous_count = c;
        previous_word = w;
    }
}

void test_Sort_ThenAddStillCounts(void) {
    add_times("a", 1);
    add_times("b", 2);
    word_list_sort_by_frequency(list);      /* b, a */

    add_times("a", 2);                      /* existing word: no duplicate */
    add_times("c", 1);                      /* new word: goes to the end */
    TEST_ASSERT_EQUAL_size_t(3, word_list_size(list));
    assert_entry(0, "b", 2);
    assert_entry(1, "a", 3);
    assert_entry(2, "c", 1);

    word_list_sort_by_frequency(list);
    assert_entry(0, "a", 3);
}

void test_Get_OutOfRangeAndNullList(void) {
    add_times("x", 1);
    TEST_ASSERT_EQUAL_INT(WORD_LIST_NOT_FOUND, word_list_get(list, 1, NULL, NULL));
    TEST_ASSERT_EQUAL_INT(WORD_LIST_ERR_ARG, word_list_get(NULL, 0, NULL, NULL));
    TEST_ASSERT_EQUAL_size_t(0, word_list_size(NULL));
    TEST_ASSERT_EQUAL_UINT(0, word_list_total(NULL));
    TEST_ASSERT_EQUAL_UINT(0, word_list_frequency(NULL, "x"));
    word_list_destroy(NULL);                /* must not crash */
}

void test_Print_AllEntries(void) {
    char text[128];

    add_times("a", 1);
    add_times("b", 2);
    word_list_sort_by_frequency(list);
    print_to_string(0, text, sizeof text);
    TEST_ASSERT_EQUAL_STRING("- b: 2\n- a: 1\n", text);
}

void test_Print_LimitedNumberOfEntries(void) {
    char text[128];

    add_times("x", 3);
    add_times("y", 2);
    add_times("z", 1);
    print_to_string(2, text, sizeof text);
    TEST_ASSERT_EQUAL_STRING("- x: 3\n- y: 2\n- ... (1 more)\n", text);
}

void test_Print_LimitLargerThanList(void) {
    char text[128];

    add_times("x", 1);
    print_to_string(5, text, sizeof text);
    TEST_ASSERT_EQUAL_STRING("- x: 1\n", text);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_Create_IsEmpty);
    RUN_TEST(test_Add_NewWordHasCountOne);
    RUN_TEST(test_Add_SameWordIncrementsCount);
    RUN_TEST(test_Add_KeepsOrderOfFirstAppearance);
    RUN_TEST(test_Add_StoresACopyOfTheWord);
    RUN_TEST(test_Add_InvalidArguments);
    RUN_TEST(test_Frequency_IsCaseSensitive);
    RUN_TEST(test_Sort_ByDecreasingFrequency);
    RUN_TEST(test_Sort_TiesInAlphabeticalOrder);
    RUN_TEST(test_Sort_EmptyAndSingleWord);
    RUN_TEST(test_Sort_ManyWords);
    RUN_TEST(test_Sort_ThenAddStillCounts);
    RUN_TEST(test_Get_OutOfRangeAndNullList);
    RUN_TEST(test_Print_AllEntries);
    RUN_TEST(test_Print_LimitedNumberOfEntries);
    RUN_TEST(test_Print_LimitLargerThanList);

    return UNITY_END();
}
