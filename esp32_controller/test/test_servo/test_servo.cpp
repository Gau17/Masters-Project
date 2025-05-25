#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/mcpwm.h"
#include "esp_log.h"
#include "esp_system.h"
#include "servo_motor_controller.h" 

void test_servo_init() {
    Servo servo_motor_1 = {
        .gpio_pin = SERVO_1_GPIO,
        .min_pulse_width_us = SERVO_MIN_PULSE_WIDTH_US,
        .max_pulse_width_us = SERVO_MAX_PULSE_WIDTH_US,
        .max_degree = SERVO_MAX_DEGREE,
        .current_angle = 0,
        .mcpwm_unit = MCPWM_UNIT_0,
        .mcpwm_timer = MCPWM_TIMER_0,
        .mcpwm_io = MCPWM0A,
        .mcpwm_op = MCPWM_OPR_A
    };
    
    TEST_ASSERT_EQUAL(ESP_OK, servo_init(&servo_motor_1, 90));
    TEST_ASSERT_EQUAL(90, servo_motor_1.current_angle);
    TEST_ASSERT_EQUAL(SERVO_MIN_PULSE_WIDTH_US, servo_motor_1.min_pulse_width_us);
    TEST_ASSERT_EQUAL(SERVO_MAX_PULSE_WIDTH_US, servo_motor_1.max_pulse_width_us);
    TEST_ASSERT_EQUAL(SERVO_MAX_DEGREE, servo_motor_1.max_degree);
    TEST_ASSERT_EQUAL(MCPWM_UNIT_0, servo_motor_1.mcpwm_unit);
    TEST_ASSERT_EQUAL(MCPWM_TIMER_0, servo_motor_1.mcpwm_timer);
    TEST_ASSERT_EQUAL(MCPWM0A, servo_motor_1.mcpwm_io);
    TEST_ASSERT_EQUAL(MCPWM_OPR_A, servo_motor_1.mcpwm_op);
    TEST_ASSERT_EQUAL(SERVO_1_GPIO, servo_motor_1.gpio_pin);
}


void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}


void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_servo_init);
    UNITY_END();
}

void loop() {
}