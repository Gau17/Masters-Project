// Working code with freertos and mcpwm
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "servo_motor_controller.h"

// Application main entry point
void setup(void) {
    ESP_LOGI(TAG, "ESP32 Servo Controller Starting");

    servo_init(&servo_motor_gripper, 110); 
    servo_init(&servo_motor_wrist_roll, 120); 
    servo_init(&servo_motor_wrist_pitch, 60); 
    servo_init(&servo_motor_elbow, 0); 
    servo_init(&servo_motor_shoulder, 0); 
    servo_init(&servo_motor_waist, 0);
    
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
    // xTaskCreate(
    //     sweep_task,            // Function
    //     "sweep_task",          // Name
    //     4096,                  // Stack size
    //     &servo_motor_gripper,  // Parameters
    //     4,                     // Priority (lower than servo task)
    //     NULL                   // Handle
    // );

    //     // Create sweep task
    // xTaskCreate(
    //     sweep_task,            // Function
    //     "sweep_task",          // Name
    //     4096,                  // Stack size
    //     &servo_motor_wrist_pitch,  // Parameters
    //     4,                     // Priority (lower than servo task)
    //     NULL                   // Handle
    // );

    //     // Create sweep task
    // xTaskCreate(
    //     sweep_task,            // Function
    //     "sweep_task",          // Name
    //     4096,                  // Stack size
    //     &servo_motor_wrist_roll,  // Parameters
    //     4,                     // Priority (lower than servo task)
    //     NULL                   // Handle
    // );

    xTaskCreate(
        sweep_task,            // Function
        "sweep_task",          // Name
        4096,                  // Stack size
        &servo_motor_elbow,  // Parameters
        4,                     // Priority (lower than servo task)
        NULL                   // Handle
    );

        // Create sweep task
    xTaskCreate(
        sweep_task,            // Function
        "sweep_task",          // Name
        4096,                  // Stack size
        &servo_motor_shoulder,  // Parameters
        4,                     // Priority (lower than servo task)
        NULL                   // Handle
    );

        // Create sweep task
    xTaskCreate(
        sweep_task,            // Function
        "sweep_task",          // Name
        4096,                  // Stack size
        &servo_motor_waist,  // Parameters
        4,                     // Priority (lower than servo task)
        NULL                   // Handle
    );
    
    ESP_LOGI(TAG, "All tasks created");
}

void loop(){}