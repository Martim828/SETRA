/* Unity tests for the linked_list module (exercise 3.2) */
#include <limits.h>
#include <stdio.h>
#include "unity.h"
#include "linked_list.h"

#define ARRAY_LEN(a)  (sizeof(a) / sizeof((a)[0]))
#define MAX_VALUES    16

static LinkedList *list;

/* Every test starts with a new, empty list */
void setUp(void) {
    list = linked_list_create();
    TEST_ASSERT_NOT_NULL(list);
}

void tearDown(void) {
    linked_list_destroy(list);
    list = NULL;
}

/* ---------------------------------------------------------------------- */
/* Helpers                                                                 */
/* ---------------------------------------------------------------------- */

static void insert_all(const int *values, size_t n) {
    for (size_t i = 0; i < n; i++) {
        TEST_ASSERT_EQUAL_INT(LINKED_LIST_OK, linked_list_insert(list, values[i]));
    }
}

/* Checks that the list holds exactly the n values in expected[] */
static void assert_list_equals(const int *expected, size_t n) {
    int actual[MAX_VALUES];
    int extra;

    TEST_ASSERT_TRUE(n <= MAX_VALUES);
    TEST_ASSERT_EQUAL_size_t(n, linked_list_size(list));
    for (size_t i = 0; i < n; i++) {
        TEST_ASSERT_EQUAL_INT(LINKED_LIST_OK, linked_list_get(list, i, &actual[i]));
    }
    if (n > 0) {
        TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, n);
    }
    TEST_ASSERT_EQUAL_INT(LINKED_LIST_NOT_FOUND, linked_list_get(list, n, &extra));
}

/* tmpfile() is standard C, but with MinGW it needs administrator rights (it
 * creates the file in C:\), so in that case a file in build/ is used */
#define SCRATCH_NAME "build/test_linked_list.tmp"

static FILE *scratch_open(void) {
    FILE *f = tmpfile();
    return f != NULL ? f : fopen(SCRATCH_NAME, "w+b");
}

static void scratch_close(FILE *f) {
    fclose(f);
    remove(SCRATCH_NAME);       /* nothing to remove if tmpfile() worked */
}

/* Prints the list into a temporary file and reads the text back */
static void print_to_string(char *buffer, size_t size) {
    FILE *f = scratch_open();
    size_t n;

    if (f == NULL) {
        TEST_IGNORE_MESSAGE("cannot create a temporary file");
        return;
    }
    linked_list_print(list, f);
    rewind(f);
    n = fread(buffer, 1, size - 1, f);
    buffer[n] = '\0';
    scratch_close(f);
}

/* ---------------------------------------------------------------------- */
/* Tests                                                                   */
/* ---------------------------------------------------------------------- */

void test_Create_ListIsEmpty(void) {
    int value;

    TEST_ASSERT_EQUAL_size_t(0, linked_list_size(list));
    TEST_ASSERT_FALSE(linked_list_contains(list, 0));
    TEST_ASSERT_EQUAL_INT(LINKED_LIST_NOT_FOUND, linked_list_get(list, 0, &value));
}

void test_Insert_AppendsAtTheEnd(void) {
    const int values[] = {10, 20, 30};

    insert_all(values, ARRAY_LEN(values));
    assert_list_equals(values, ARRAY_LEN(values));
}

void test_Insert_NegativeRepeatedAndLimitValues(void) {
    const int values[] = {-5, 0, -5, INT_MAX, INT_MIN};

    insert_all(values, ARRAY_LEN(values));
    assert_list_equals(values, ARRAY_LEN(values));
}

void test_Delete_Head(void) {
    const int values[] = {1, 2, 3};
    const int expected[] = {2, 3};

    insert_all(values, ARRAY_LEN(values));
    TEST_ASSERT_EQUAL_INT(LINKED_LIST_OK, linked_list_delete(list, 1));
    assert_list_equals(expected, ARRAY_LEN(expected));
}

void test_Delete_Middle(void) {
    const int values[] = {1, 2, 3};
    const int expected[] = {1, 3};

    insert_all(values, ARRAY_LEN(values));
    TEST_ASSERT_EQUAL_INT(LINKED_LIST_OK, linked_list_delete(list, 2));
    assert_list_equals(expected, ARRAY_LEN(expected));
}

void test_Delete_Tail_ThenInsertGoesToTheEnd(void) {
    const int values[] = {1, 2, 3};
    const int expected[] = {1, 2, 4};

    insert_all(values, ARRAY_LEN(values));
    TEST_ASSERT_EQUAL_INT(LINKED_LIST_OK, linked_list_delete(list, 3));
    /* the internal tail pointer must now be the node with 2 */
    TEST_ASSERT_EQUAL_INT(LINKED_LIST_OK, linked_list_insert(list, 4));
    assert_list_equals(expected, ARRAY_LEN(expected));
}

void test_Delete_OnlyElement_ThenInsert(void) {
    const int expected[] = {9};

    TEST_ASSERT_EQUAL_INT(LINKED_LIST_OK, linked_list_insert(list, 8));
    TEST_ASSERT_EQUAL_INT(LINKED_LIST_OK, linked_list_delete(list, 8));
    assert_list_equals(NULL, 0);

    TEST_ASSERT_EQUAL_INT(LINKED_LIST_OK, linked_list_insert(list, 9));
    assert_list_equals(expected, ARRAY_LEN(expected));
}

void test_Delete_RemovesOnlyTheFirstOccurrence(void) {
    const int values[] = {7, 8, 7};
    const int expected[] = {8, 7};

    insert_all(values, ARRAY_LEN(values));
    TEST_ASSERT_EQUAL_INT(LINKED_LIST_OK, linked_list_delete(list, 7));
    assert_list_equals(expected, ARRAY_LEN(expected));
}

void test_Delete_MissingValue_LeavesListUnchanged(void) {
    const int values[] = {1, 2, 3};

    insert_all(values, ARRAY_LEN(values));
    TEST_ASSERT_EQUAL_INT(LINKED_LIST_NOT_FOUND, linked_list_delete(list, 42));
    assert_list_equals(values, ARRAY_LEN(values));
}

void test_Delete_FromEmptyList(void) {
    TEST_ASSERT_EQUAL_INT(LINKED_LIST_NOT_FOUND, linked_list_delete(list, 1));
    assert_list_equals(NULL, 0);
}

void test_Contains(void) {
    const int values[] = {4, 5, 6};

    insert_all(values, ARRAY_LEN(values));
    TEST_ASSERT_TRUE(linked_list_contains(list, 4));
    TEST_ASSERT_TRUE(linked_list_contains(list, 6));
    TEST_ASSERT_FALSE(linked_list_contains(list, 7));

    linked_list_delete(list, 6);
    TEST_ASSERT_FALSE(linked_list_contains(list, 6));
}

void test_Get_OutOfRange(void) {
    int value = 123;

    TEST_ASSERT_EQUAL_INT(LINKED_LIST_OK, linked_list_insert(list, 1));
    TEST_ASSERT_EQUAL_INT(LINKED_LIST_NOT_FOUND, linked_list_get(list, 1, &value));
    TEST_ASSERT_EQUAL_INT(LINKED_LIST_NOT_FOUND, linked_list_get(list, 1000, &value));
    TEST_ASSERT_EQUAL_INT(123, value);      /* not modified on failure */
}

void test_Print_EmptyList(void) {
    char text[64];

    print_to_string(text, sizeof text);
    TEST_ASSERT_EQUAL_STRING("[]\n", text);
}

void test_Print_Elements(void) {
    const int values[] = {1, -2, 3};
    char text[64];

    insert_all(values, ARRAY_LEN(values));
    print_to_string(text, sizeof text);
    TEST_ASSERT_EQUAL_STRING("[1 -> -2 -> 3]\n", text);
}

void test_NullArguments(void) {
    int value;

    TEST_ASSERT_EQUAL_INT(LINKED_LIST_ERR_ARG, linked_list_insert(NULL, 1));
    TEST_ASSERT_EQUAL_INT(LINKED_LIST_ERR_ARG, linked_list_delete(NULL, 1));
    TEST_ASSERT_EQUAL_INT(LINKED_LIST_ERR_ARG, linked_list_get(NULL, 0, &value));
    TEST_ASSERT_EQUAL_INT(LINKED_LIST_ERR_ARG, linked_list_get(list, 0, NULL));
    TEST_ASSERT_EQUAL_size_t(0, linked_list_size(NULL));
    TEST_ASSERT_FALSE(linked_list_contains(NULL, 1));
    linked_list_destroy(NULL);              /* must not crash */
}

void test_ManyElements(void) {
    int value;

    for (int i = 0; i < 1000; i++) {
        TEST_ASSERT_EQUAL_INT(LINKED_LIST_OK, linked_list_insert(list, i));
    }
    TEST_ASSERT_EQUAL_size_t(1000, linked_list_size(list));

    for (int i = 0; i < 1000; i += 2) {     /* delete the even values */
        TEST_ASSERT_EQUAL_INT(LINKED_LIST_OK, linked_list_delete(list, i));
    }
    TEST_ASSERT_EQUAL_size_t(500, linked_list_size(list));

    for (size_t i = 0; i < 500; i++) {
        TEST_ASSERT_EQUAL_INT(LINKED_LIST_OK, linked_list_get(list, i, &value));
        TEST_ASSERT_EQUAL_INT((int)(2 * i + 1), value);
    }
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_Create_ListIsEmpty);
    RUN_TEST(test_Insert_AppendsAtTheEnd);
    RUN_TEST(test_Insert_NegativeRepeatedAndLimitValues);
    RUN_TEST(test_Delete_Head);
    RUN_TEST(test_Delete_Middle);
    RUN_TEST(test_Delete_Tail_ThenInsertGoesToTheEnd);
    RUN_TEST(test_Delete_OnlyElement_ThenInsert);
    RUN_TEST(test_Delete_RemovesOnlyTheFirstOccurrence);
    RUN_TEST(test_Delete_MissingValue_LeavesListUnchanged);
    RUN_TEST(test_Delete_FromEmptyList);
    RUN_TEST(test_Contains);
    RUN_TEST(test_Get_OutOfRange);
    RUN_TEST(test_Print_EmptyList);
    RUN_TEST(test_Print_Elements);
    RUN_TEST(test_NullArguments);
    RUN_TEST(test_ManyElements);

    return UNITY_END();
}
