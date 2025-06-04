#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/mcpwm.h"
#include <stdint.h>
#include "esp_log.h"
#include "servo_motor_controller.h"

// Servo servo_motor_gripper = {
//     .gpio_pin = SERVO_1_GPIO,
//     .min_pulse_width_us = SERVO_MIN_PULSE_WIDTH_US,
//     .max_pulse_width_us = SERVO_MAX_PULSE_WIDTH_US,
//     .max_degree = SERVO_MAX_DEGREE,
//     .current_angle = 0,
//     .mcpwm_unit = MCPWM_UNIT_0,
//     .mcpwm_timer = MCPWM_TIMER_0,
//     .mcpwm_io = MCPWM0A,
//     .mcpwm_op = MCPWM_OPR_A
// };

// Servo servo_motor_wrist_roll = {
//     .gpio_pin = SERVO_2_GPIO,
//     .min_pulse_width_us = SERVO_MIN_PULSE_WIDTH_US,
//     .max_pulse_width_us = SERVO_MAX_PULSE_WIDTH_US,
//     .max_degree = SERVO_MAX_DEGREE,
//     .current_angle = 0,
//     .mcpwm_unit = MCPWM_UNIT_0,
//     .mcpwm_timer = MCPWM_TIMER_0,
//     .mcpwm_io = MCPWM0A,
//     .mcpwm_op = MCPWM_OPR_A
// };

// Servo servo_motor_wrist_pitch = {
//     .gpio_pin = SERVO_3_GPIO,
//     .min_pulse_width_us = SERVO_MIN_PULSE_WIDTH_US,
//     .max_pulse_width_us = SERVO_MAX_PULSE_WIDTH_US,
//     .max_degree = SERVO_MAX_DEGREE,
//     .current_angle = 0,
//     .mcpwm_unit = MCPWM_UNIT_0,
//     .mcpwm_timer = MCPWM_TIMER_0,
//     .mcpwm_io = MCPWM0A,
//     .mcpwm_op = MCPWM_OPR_A
// };

// Servo servo_motor_elbow = { //Initial angle 
//     .gpio_pin = SERVO_4_GPIO,
//     .min_pulse_width_us = SERVO_MIN_PULSE_WIDTH_US,
//     .max_pulse_width_us = SERVO_MAX_PULSE_WIDTH_US,
//     .max_degree = SERVO_MAX_DEGREE,
//     .current_angle = 0,
//     .mcpwm_unit = MCPWM_UNIT_0,
//     .mcpwm_timer = MCPWM_TIMER_0,
//     .mcpwm_io = MCPWM0A,
//     .mcpwm_op = MCPWM_OPR_A
// };

// Servo servo_motor_shoulder = { //Initial angle 45 ish degrees
//     .gpio_pin = SERVO_5_GPIO,
//     .min_pulse_width_us = SERVO_MIN_PULSE_WIDTH_US,
//     .max_pulse_width_us = SERVO_MAX_PULSE_WIDTH_US,
//     .max_degree = SERVO_MAX_DEGREE,
//     .current_angle = 0,
//     .mcpwm_unit = MCPWM_UNIT_0,
//     .mcpwm_timer = MCPWM_TIMER_0,
//     .mcpwm_io = MCPWM0A,
//     .mcpwm_op = MCPWM_OPR_A
// };

// Servo servo_motor_waist = { // Initial angle 100 degrees
//     .gpio_pin = SERVO_6_GPIO,
//     .min_pulse_width_us = SERVO_MIN_PULSE_WIDTH_US,
//     .max_pulse_width_us = SERVO_MAX_PULSE_WIDTH_US,
//     .max_degree = SERVO_MAX_DEGREE,
//     .current_angle = 0,
//     .mcpwm_unit = MCPWM_UNIT_0,
//     .mcpwm_timer = MCPWM_TIMER_0,
//     .mcpwm_io = MCPWM0A,
//     .mcpwm_op = MCPWM_OPR_A
// };

Servo servo_motor_gripper = {
    .gpio_pin = SERVO_1_GPIO,
    .min_pulse_width_us = SERVO_MIN_PULSE_WIDTH_US,
    .max_pulse_width_us = SERVO_MAX_PULSE_WIDTH_US,
    .max_degree = SERVO_MAX_DEGREE,
    .current_angle = 0,
    .mcpwm_unit = MCPWM_UNIT_0,
    .mcpwm_timer = MCPWM_TIMER_0,
    .mcpwm_io = MCPWM0A,        // Corresponds to Timer 0, Operator A
    .mcpwm_op = MCPWM_OPR_A
};

Servo servo_motor_wrist_roll = {
    .gpio_pin = SERVO_2_GPIO,
    .min_pulse_width_us = SERVO_MIN_PULSE_WIDTH_US,
    .max_pulse_width_us = SERVO_MAX_PULSE_WIDTH_US,
    .max_degree = SERVO_MAX_DEGREE,
    .current_angle = 0,
    .mcpwm_unit = MCPWM_UNIT_0,
    .mcpwm_timer = MCPWM_TIMER_0, // Same Timer 0
    .mcpwm_io = MCPWM0B,        // But use Operator B
    .mcpwm_op = MCPWM_OPR_B
};

Servo servo_motor_wrist_pitch = {
    .gpio_pin = SERVO_3_GPIO,
    .min_pulse_width_us = SERVO_MIN_PULSE_WIDTH_US,
    .max_pulse_width_us = SERVO_MAX_PULSE_WIDTH_US,
    .max_degree = SERVO_MAX_DEGREE,
    .current_angle = 0,
    .mcpwm_unit = MCPWM_UNIT_0,
    .mcpwm_timer = MCPWM_TIMER_1, // Use Timer 1
    .mcpwm_io = MCPWM1A,        // Operator A of Timer 1
    .mcpwm_op = MCPWM_OPR_A
};

Servo servo_motor_elbow = { 
    .gpio_pin = SERVO_4_GPIO,
    .min_pulse_width_us = SERVO_MIN_PULSE_WIDTH_US,
    .max_pulse_width_us = SERVO_MAX_PULSE_WIDTH_US,
    .max_degree = SERVO_MAX_DEGREE,
    .current_angle = 0,
    .mcpwm_unit = MCPWM_UNIT_0,
    .mcpwm_timer = MCPWM_TIMER_1, // Timer 1
    .mcpwm_io = MCPWM1B,        // Operator B of Timer 1
    .mcpwm_op = MCPWM_OPR_B
};

Servo servo_motor_shoulder = { 
    .gpio_pin = SERVO_5_GPIO,
    .min_pulse_width_us = SERVO_MIN_PULSE_WIDTH_US,
    .max_pulse_width_us = SERVO_MAX_PULSE_WIDTH_US,
    .max_degree = SERVO_MAX_DEGREE,
    .current_angle = 0,
    .mcpwm_unit = MCPWM_UNIT_0,
    .mcpwm_timer = MCPWM_TIMER_2, // Timer 2
    .mcpwm_io = MCPWM2A,        // Operator A of Timer 2
    .mcpwm_op = MCPWM_OPR_A
};

Servo servo_motor_waist = { 
    .gpio_pin = SERVO_6_GPIO,
    .min_pulse_width_us = SERVO_MIN_PULSE_WIDTH_US,
    .max_pulse_width_us = SERVO_MAX_PULSE_WIDTH_US,
    .max_degree = SERVO_MAX_DEGREE,
    .current_angle = 0,
    .mcpwm_unit = MCPWM_UNIT_0,
    .mcpwm_timer = MCPWM_TIMER_2, // Timer 2
    .mcpwm_io = MCPWM2B,        // Operator B of Timer 2
    .mcpwm_op = MCPWM_OPR_B
};

const char *TAG = "servo_controller";

QueueHandle_t servo_cmd_queue = NULL;

// Helper function to convert angle to pulse width
static uint32_t servo_per_degree_init(Servo* servo, uint32_t degree) {
    return (((servo->max_pulse_width_us - servo->min_pulse_width_us) * degree) / 
            servo->max_degree + servo->min_pulse_width_us);
}

esp_err_t servo_init(Servo* servo, uint32_t initial_angle) {
    ESP_LOGI(TAG, "Initializing servo motor on GPIO %d to %d degrees", servo->gpio_pin, initial_angle);
    
    servo->current_angle = initial_angle;

    mcpwm_gpio_init(servo->mcpwm_unit, servo->mcpwm_io, servo->gpio_pin);
    
    mcpwm_config_t pwm_config = {
        .frequency = 50, 
        .cmpr_a = 0,     
        .cmpr_b = 0,     
        .duty_mode = MCPWM_DUTY_MODE_0,
        .counter_mode = MCPWM_UP_COUNTER,
    };
    
    mcpwm_init(servo->mcpwm_unit, servo->mcpwm_timer, &pwm_config);

    uint32_t pulse_width = servo_per_degree_init(servo, servo->current_angle);
    mcpwm_set_duty_in_us(servo->mcpwm_unit, servo->mcpwm_timer, servo->mcpwm_op, pulse_width);
    
    ESP_LOGI(TAG, "Servo initialized at position %d degrees", servo->current_angle);
    return ESP_OK; 
}

void servo_set_angle(Servo* servo, uint32_t angle) { 
    if (angle > servo->max_degree) { 
        angle = servo->max_degree;
    }
    
    // Convert angle to pulse width and set duty cycle
    uint32_t pulse_width = servo_per_degree_init(servo, angle);
    ESP_LOGI(TAG, "Setting servo %d to angle: %d degrees (pulse width: %d us)", servo->gpio_pin, angle, pulse_width);
    mcpwm_set_duty_in_us(servo->mcpwm_unit, servo->mcpwm_timer, servo->mcpwm_op, pulse_width);
    servo->current_angle = angle;
}

// Servo control task for testing
void servo_control_task(void *arg) {
    Servo_cmd cmd;
    
    ESP_LOGI(TAG, "Servo control task started");
    
    while (1) {
        if (xQueueReceive(servo_cmd_queue, &cmd, portMAX_DELAY) == pdTRUE) {
            servo_set_angle(cmd.servo, cmd.angle);
            vTaskDelay(cmd.delay_ms / portTICK_PERIOD_MS);
        }
    }
}

// Sweep generation task for testing
void sweep_task(void *arg) {
    Servo* servo_to_sweep = (Servo*)arg;
    if (servo_to_sweep == NULL) {
        ESP_LOGE(TAG, "Sweep task received NULL servo pointer");
        vTaskDelete(NULL);
        return;
    }

    Servo_cmd cmd;
    bool direction = true;
    uint32_t current_angle = 0;
    
    ESP_LOGI(TAG, "Sweep task started");
    
    while (1) {
        cmd.servo = servo_to_sweep;
        cmd.angle = current_angle;
        cmd.delay_ms = 50;
        if (xQueueSend(servo_cmd_queue, &cmd, 0) != pdTRUE) {
            ESP_LOGW(TAG, "Failed to send command to servo queue");
        }
        // Update the angle for the next iteration
        if (direction) {
            current_angle += 10;
            if (current_angle >= 120) {
                current_angle = 120;
                direction = false;
            }
        } else {
            current_angle -= 10;
            if (current_angle <= 0 || current_angle > 120) {
                current_angle = 0;
                direction = true;
            }
        }
        
        vTaskDelay(150 / portTICK_PERIOD_MS);
    }
}