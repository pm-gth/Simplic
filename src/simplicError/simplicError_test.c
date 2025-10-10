#include "unity.h"
#include "unity_internals.h"

#include "simplicError.h"

void setUp(void) {
    ;
}

void tearDown(void) {
    ;
}

void testErrorSet(void){
    SimplicError* error = initError();
    setError(error, "This is a test error", ERROR_DIVISION_BY_ZERO);

    TEST_ASSERT_TRUE(error->hasError);
    TEST_ASSERT_EQUAL_STRING("This is a test error", error->errMsg);
    TEST_ASSERT_EQUAL_INT(ERROR_DIVISION_BY_ZERO, error->errCode);

    deleteError(&error);
}

void testErrorOverride(void){
    SimplicError* error = initError();
    setError(error, "This is a test error", ERROR_DIVISION_BY_ZERO);
    setError(error, "New error", ERROR_UNKNOWN_INSTRUCTION);

    TEST_ASSERT_TRUE(error->hasError);
    TEST_ASSERT_EQUAL_STRING("New error", error->errMsg);
    TEST_ASSERT_EQUAL_INT(ERROR_UNKNOWN_INSTRUCTION, error->errCode);

    deleteError(&error);
}

void testErrorUnset(void){
    SimplicError* error = initError();
    setError(error, "This is a test error", ERROR_DIVISION_BY_ZERO);
    unsetError(error);

    TEST_ASSERT_FALSE(error->hasError);
    TEST_ASSERT_TRUE(error->errMsg == NULL);
    TEST_ASSERT_EQUAL_INT(NO_ERROR, error->errCode);

    deleteError(&error);
}

void testErrorDelete(void){
    SimplicError* error = initError();
    setError(error, "This is a test error", ERROR_DIVISION_BY_ZERO);

    deleteError(&error);
    TEST_ASSERT_TRUE(error == NULL);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testErrorSet);
    RUN_TEST(testErrorOverride);
    RUN_TEST(testErrorUnset);
    RUN_TEST(testErrorDelete);
    return UNITY_END();
}
