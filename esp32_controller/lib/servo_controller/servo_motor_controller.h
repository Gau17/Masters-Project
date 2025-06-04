/**
 * @file servo_motor_controller.h
 * @brief Servo motor control library for ESP32 using MCPWM
 * 
 * This library provides functions and task implementations to control
 * servo motors on ESP32 microcontrollers using the MCPWM peripheral.
 * 
 * @author Gautam Bidari
 */

#ifndef SERVO_MOTOR_CONTROLLER_H
#define SERVO_MOTOR_CONTROLLER_H

#include <stdio.h>
#include <stdint.h>
#include "freertos/queue.h"
#include "driver/mcpwm.h"

// #ifdef __cplusplus
// extern "C" {
// #endif

/**
 * @brief Hardware and servo motor configuration parameters
 */
#define SERVO_1_GPIO 18                  /**< GPIO pin connected to the servo pwm wire */
#define SERVO_2_GPIO 19                  /**< GPIO pin connected to the servo pwm wire */
#define SERVO_3_GPIO 21                  /**< GPIO pin connected to the servo pwm wire */
#define SERVO_4_GPIO 27                  /**< GPIO pin connected to the servo pwm wire */
#define SERVO_5_GPIO 26                  /**< GPIO pin connected to the servo pwm wire */
#define SERVO_6_GPIO 25                  /**< GPIO pin connected to the servo pwm wire */
#define SERVO_MIN_PULSE_WIDTH_US 500   /**< Minimum pulse width in microseconds (servo at 0 degrees) */
#define SERVO_MAX_PULSE_WIDTH_US 2400  /**< Maximum pulse width in microseconds (servo at 180 degrees) */
#define SERVO_MAX_DEGREE 120           /**< Maximum angle in degrees the servo can rotate */

/**
 * @brief FreeRTOS task parameters for the servo control tasks
 */
#define SERVO_TASK_PRIORITY 5          /**< Priority level for servo control task */
#define SERVO_TASK_STACK_SIZE 4096     /**< Stack size in bytes for servo control task */

/**
 * @brief Logging tag used for ESP_LOG macros
 */
extern const char *TAG;

/**
 * @brief Servo object structure
 * 
 * Contains configuration and state for a single servo motor
 */
struct Servo {
    uint8_t gpio_pin;               /**< GPIO pin number the servo is connected to */
    uint16_t min_pulse_width_us;    /**< Minimum pulse width in microseconds (0 degrees) */
    uint16_t max_pulse_width_us;    /**< Maximum pulse width in microseconds (max degrees) */
    uint16_t max_degree;            /**< Maximum angle in degrees */
    uint32_t current_angle;         /**< Current angle of the servo */
    mcpwm_unit_t mcpwm_unit;        /**< MCPWM unit to use (MCPWM_UNIT_0, MCPWM_UNIT_1) */
    mcpwm_timer_t mcpwm_timer;      /**< MCPWM timer to use (MCPWM_TIMER_0, MCPWM_TIMER_1, MCPWM_TIMER_2) */
    mcpwm_io_signals_t mcpwm_io;    /**< MCPWM signal (MCPWM0A, MCPWM0B, etc.) */
    mcpwm_operator_t mcpwm_op;      /**< MCPWM operator (MCPWM_OPR_A, MCPWM_OPR_B) */
};

extern Servo servo_motor_gripper;
extern Servo servo_motor_wrist_roll;
extern Servo servo_motor_wrist_pitch;
extern Servo servo_motor_elbow;
extern Servo servo_motor_shoulder;
extern Servo servo_motor_waist;

/**
 * @brief Queue for sending commands to the servo control task
 * 
 * This queue is used to send commands from any task to the servo motor control task.
 * Commands include target angle and delay time.
 */
extern QueueHandle_t servo_cmd_queue;

/**
 * @brief Command structure for servo motor control
 * 
 * Contains parameters needed to position the servo motor.
 */
struct Servo_cmd{
    uint32_t angle;    /**< Target angle in degrees (0-180) */
    uint32_t delay_ms; /**< Delay after reaching position, in milliseconds */
    Servo* servo;    /**< Pointer to the servo to control */
};

/**
 * @brief Initialize a servo motor
 * 
 * Sets up the MCPWM for controlling a servo motor with the specified parameters.
 * 
 * @param servo Pointer to a servo structure containing servo configuration
 * @param initial_angle Initial angle to set the servo to (0-180 degrees)
 * @return esp_err_t ESP_OK on success, or an error code on failure
 */
esp_err_t servo_init(Servo* servo, uint32_t initial_angle);

/**
 * @brief Set the angle of a servo motor
 * 
 * Moves the servo to the specified angle.
 * 
 * @param servo Pointer to the servo to control
 * @param angle Target angle in degrees (0-180)
 */
void servo_set_angle(Servo* servo, uint32_t angle);

/**
 * @brief Servo control task that processes commands from the queue
 * 
 * This FreeRTOS task initializes the servo hardware and then continuously
 * monitors the servo command queue. When a new command arrives, it sets 
 * the servo to the specified angle and waits for the specified delay time
 * before processing the next command.
 */
void servo_control_task(void *arg);

//For basic testing
void sweep_task(void *arg);

// #ifdef __cplusplus
// }
// #endif

#endif /* SERVO_MOTOR_CONTROLLER_H */