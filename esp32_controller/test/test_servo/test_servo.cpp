#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/mcpwm.h"
#include "esp_log.h"
#include "esp_system.h"
#include "servo_motor_controller.h" 

void test_servo_init() {

    // esp_err_t res = servo_init(&servo_motor_gripper, 0);
     
    // TEST_ASSERT_EQUAL(ESP_OK, res);
    TEST_ASSERT_EQUAL(0, servo_motor_gripper.current_angle);
    TEST_ASSERT_EQUAL(SERVO_MIN_PULSE_WIDTH_US, servo_motor_gripper.min_pulse_width_us);
    TEST_ASSERT_EQUAL(SERVO_MAX_PULSE_WIDTH_US, servo_motor_gripper.max_pulse_width_us);
    TEST_ASSERT_EQUAL(SERVO_MAX_DEGREE, servo_motor_gripper.max_degree);
    TEST_ASSERT_EQUAL(MCPWM_UNIT_0, servo_motor_gripper.mcpwm_unit);
    TEST_ASSERT_EQUAL(MCPWM_TIMER_0, servo_motor_gripper.mcpwm_timer);
    TEST_ASSERT_EQUAL(MCPWM0A, servo_motor_gripper.mcpwm_io);
    TEST_ASSERT_EQUAL(MCPWM_OPR_A, servo_motor_gripper.mcpwm_op);
    TEST_ASSERT_EQUAL(SERVO_1_GPIO, servo_motor_gripper.gpio_pin);
}

void test_servo_set_angle(){

    servo_set_angle(&servo_motor_gripper, 0);
    TEST_ASSERT_EQUAL(0, servo_motor_gripper.current_angle);

    servo_set_angle(&servo_motor_wrist_pitch, 0);
    TEST_ASSERT_EQUAL(0, servo_motor_wrist_pitch.current_angle);

    servo_set_angle(&servo_motor_wrist_roll, 120);
    TEST_ASSERT_EQUAL(120, servo_motor_wrist_roll.current_angle);

    servo_set_angle(&servo_motor_elbow, 100);
    TEST_ASSERT_EQUAL(100, servo_motor_elbow.current_angle);

    servo_set_angle(&servo_motor_shoulder, 80);
    TEST_ASSERT_EQUAL(80, servo_motor_shoulder.current_angle);

    servo_set_angle(&servo_motor_waist, 70);
    TEST_ASSERT_EQUAL(70, servo_motor_waist.current_angle);
}

void test_apply_saved_pose() {
    saved_poses[0].gripper_angle = 0;
    saved_poses[0].wrist_pitch_angle = 0;
    saved_poses[0].wrist_roll_angle = 120;
    saved_poses[0].elbow_angle = 100;
    saved_poses[0].shoulder_angle = 90;
    saved_poses[0].waist_angle = 70;

    saved_poses[1].gripper_angle = 60;
    saved_poses[1].wrist_pitch_angle = 20;
    saved_poses[1].wrist_roll_angle = 120;
    saved_poses[1].elbow_angle = 120;
    saved_poses[1].shoulder_angle = 90;
    saved_poses[1].waist_angle = 70;

    RobotPose pose_to_apply = saved_poses[0];
        
    servo_set_angle(&servo_motor_gripper, pose_to_apply.gripper_angle);
    servo_set_angle(&servo_motor_wrist_pitch, pose_to_apply.wrist_pitch_angle);
    servo_set_angle(&servo_motor_wrist_roll, pose_to_apply.wrist_roll_angle);
    servo_set_angle(&servo_motor_elbow, pose_to_apply.elbow_angle);
    servo_set_angle(&servo_motor_shoulder, pose_to_apply.shoulder_angle);
    servo_set_angle(&servo_motor_waist, pose_to_apply.waist_angle);
    vTaskDelay(pdMS_TO_TICKS(3500));

    pose_to_apply = saved_poses[1];

    servo_set_angle(&servo_motor_gripper, pose_to_apply.gripper_angle);
    servo_set_angle(&servo_motor_wrist_pitch, pose_to_apply.wrist_pitch_angle);
    servo_set_angle(&servo_motor_wrist_roll, pose_to_apply.wrist_roll_angle);
    servo_set_angle(&servo_motor_elbow, pose_to_apply.elbow_angle);
    servo_set_angle(&servo_motor_shoulder, pose_to_apply.shoulder_angle);
    servo_set_angle(&servo_motor_waist, pose_to_apply.waist_angle);
    vTaskDelay(pdMS_TO_TICKS(1000));
}


void setUp(void) {
    // set stuff up here
    esp_err_t res = servo_init(&servo_motor_gripper, 0);
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, res, "Servo initialization failed in setUp");

    res = servo_init(&servo_motor_wrist_pitch, 0);
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, res, "Servo initialization failed in setUp");

    res = servo_init(&servo_motor_wrist_roll, 120);
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, res, "Servo initialization failed in setUp");

    res = servo_init(&servo_motor_elbow, 0);
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, res, "Servo initialization failed in setUp");

    res = servo_init(&servo_motor_shoulder, 0);
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, res, "Servo initialization failed in setUp");

    res = servo_init(&servo_motor_waist, 0);
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, res, "Servo initialization failed in setUp");

}

void tearDown(void) {
    // clean stuff up here
    // mcpwm_stop(servo_motor_gripper.mcpwm_unit, servo_motor_gripper.mcpwm_timer);

}


void setup() {
    vTaskDelay(pdMS_TO_TICKS(2000));
    UNITY_BEGIN();
    RUN_TEST(test_servo_init);
    RUN_TEST(test_servo_set_angle);
    // RUN_TEST(test_set_new_pose);
    RUN_TEST(test_apply_saved_pose);
    UNITY_END();
}

void loop() {
}