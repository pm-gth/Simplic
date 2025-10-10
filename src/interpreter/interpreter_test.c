#include "unity.h"
#include "unity_internals.h"

#include "interpreter.h"

void setUp(void) {
    ;
}

void tearDown(void) {
    ;
}

void hello(void) {
    TEST_ASSERT_TRUE(1);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(hello);
    return UNITY_END();
}
