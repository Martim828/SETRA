/* Unity tests for the configData module (exercise 1.3) */
#include <stdint.h>
#include "unity.h"
#include "configData.h"

/* Runs before every test: each test starts from a known state */
void setUp(void) {
    configInit();
}

void tearDown(void) {
}

void test_configInit_SetsZero(void) {
    TEST_ASSERT_EQUAL_UINT16(0, configGetMaxConnections());
}

void test_configInit_ResetsPreviousValue(void) {
    configSetMaxConnections(42);
    configInit();
    TEST_ASSERT_EQUAL_UINT16(0, configGetMaxConnections());
}

void test_configSet_ThenGetReturnsSameValue(void) {
    configSetMaxConnections(100);
    TEST_ASSERT_EQUAL_UINT16(100, configGetMaxConnections());
}

void test_configSet_LimitValues(void) {
    configSetMaxConnections(UINT16_MAX);
    TEST_ASSERT_EQUAL_UINT16(UINT16_MAX, configGetMaxConnections());

    configSetMaxConnections(0);
    TEST_ASSERT_EQUAL_UINT16(0, configGetMaxConnections());
}

void test_configSet_LastValueWins(void) {
    configSetMaxConnections(10);
    configSetMaxConnections(20);
    configSetMaxConnections(30);
    TEST_ASSERT_EQUAL_UINT16(30, configGetMaxConnections());
}

void test_configGet_DoesNotChangeValue(void) {
    configSetMaxConnections(7);
    (void)configGetMaxConnections();
    (void)configGetMaxConnections();
    TEST_ASSERT_EQUAL_UINT16(7, configGetMaxConnections());
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_configInit_SetsZero);
    RUN_TEST(test_configInit_ResetsPreviousValue);
    RUN_TEST(test_configSet_ThenGetReturnsSameValue);
    RUN_TEST(test_configSet_LimitValues);
    RUN_TEST(test_configSet_LastValueWins);
    RUN_TEST(test_configGet_DoesNotChangeValue);

    return UNITY_END();
}
