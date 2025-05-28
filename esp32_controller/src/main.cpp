// Initial attempt with ESP32Servo arduino library

// #include <stdio.h>
// #include "ESP32Servo.h"
// // #include "freertos/FreeRTOS.h"
// // #include "freertos/task.h"

// #define SERVO_PIN 18

// Servo myservo;

// void setup(void)
// {
//     Serial.begin(115200);

//     Serial.println("Hello from ESP32!");

//     myservo.attach(SERVO_PIN);

//     myservo.write(0);

//     Serial.println("Servo initialized at position 0°");
// }

// void loop(void)
// {
//     // Move servo from 0 to 180 degrees
//     for (int pos = 0; pos <= 180; pos += 10) {
//         myservo.write(pos);
//         Serial.printf("Servo position: %d degrees\n", pos);
//         vTaskDelay(50 / portTICK_PERIOD_MS);
//     }
    
//     // Move servo from 180 to 0 degrees
//     for (int pos = 180; pos >= 0; pos -= 10) {
//         myservo.write(pos);
//         Serial.printf("Servo position: %d degrees\n", pos);
//         vTaskDelay(50 / portTICK_PERIOD_MS);
//     }
// }

// Working code with freertos and mcpwm
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "servo_motor_controller.h"

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

// Application main entry point
void setup(void) {
    ESP_LOGI(TAG, "ESP32 Servo Controller Starting");

    servo_init(&servo_motor_1, 0); // Initialize servo motor to 0 degrees
    
    // Create command queue for servo
    servo_cmd_queue = xQueueCreate(10, sizeof(Servo_cmd));
    if (servo_cmd_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create servo command queue");
        return;
    }
    
    // Create servo control task
    xTaskCreate(
        servo_control_task,    // Function
        "servo_control",       // Name
        SERVO_TASK_STACK_SIZE, // Stack size
        NULL,                  // Parameters
        SERVO_TASK_PRIORITY,   // Priority
        NULL                   // Handle
    );
    
    // Create sweep task
    xTaskCreate(
        sweep_task,            // Function
        "sweep_task",          // Name
        4096,                  // Stack size
        &servo_motor_1,        // Parameters
        4,                     // Priority (lower than servo task)
        NULL                   // Handle
    );
    
    ESP_LOGI(TAG, "All tasks created");
}

void loop(){}