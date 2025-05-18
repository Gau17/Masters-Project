#include <unity.h>

// Define a function to be tested (example)
int add(int a, int b) {
    return a + b;
}

// Test case for the add function
void test_function_add(void) {
    TEST_ASSERT_EQUAL_INT(3, add(1, 2));
    TEST_ASSERT_EQUAL_INT(-1, add(1, -2));
}

// This function is called by the Unity test runner
void app_main() {
    UNITY_BEGIN();
    RUN_TEST(test_function_add);
    // Add more RUN_TEST calls for other test cases
    UNITY_END();
}
