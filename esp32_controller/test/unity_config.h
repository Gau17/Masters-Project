// Configuration for Unity Test Framework
#ifndef UNITY_CONFIG_H
#define UNITY_CONFIG_H

// Define our own assert macros for easier testing
#define TEST_ASSERT_TRUE_MESSAGE(condition, message) do { TEST_ASSERT_TRUE(condition); } while(0)
#define TEST_ASSERT_EQUAL_UINT8(expected, actual) do { TEST_ASSERT_EQUAL(expected, actual); } while(0)
#define TEST_PASS_MESSAGE(message) do { TEST_PASS(); } while(0)

#endif // UNITY_CONFIG_HPP
