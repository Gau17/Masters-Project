// Working code with freertos and mcpwm
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "servo_motor_controller.h"
#include "wifi_comms.h"
#include "coap_client.h"
#include "coap_server.h"

// Application main entry point
void setup(void) {
    Serial.begin(115200);
    ESP_LOGI(TAG, "ESP32 Servo Controller Starting");

    // Initialize servo motor to 0 degrees
    servo_init(&servo_motor_1, 0); 

    // Initialize WiFi Communications
    ESP_LOGI(TAG, "Initializing WiFi Communications...");
    wifi_comms_init(MAIN_WIFI_SSID, MAIN_WIFI_PASSWORD);
    ESP_LOGI(TAG, "WiFi Communications Initialized.");

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

    coap_client_init();

    // coap_server_init();
    
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

    // xTaskCreate(
    //     coap_client_task,       // Function
    //     "coap_client_task",     // Name
    //     4096,                   // Stack size
    //     NULL,                   // Parameters
    //     3,                      // Priority
    //     NULL                    // Handle
    // );

    // xTaskCreate(
    //     coap_get_request_test,
    //     "coap_get_request_test",
    //     4096,
    //     NULL,
    //     2,
    //     NULL
    // );

    // xTaskCreate(
    //     coap_post_request_test,
    //     "coap_post_request_test",
    //     4096,
    //     NULL,
    //     1,
    //     NULL
    // );

    xTaskCreate(
        coap_servo_cmd_test_task,
        "coap_servo_cmd_test_task",
        4096,
        NULL,
        1,
        NULL
    );

    // xTaskCreate(
    //     coap_server_task,       // Function
    //     "coap_server_task",     // Name
    //     4096,                   // Stack size
    //     NULL,                   // Parameters
    //     3,                      // Priority
    //     NULL                    // Handle
    // );

    //     xTaskCreate(
    //     coap_basic_notify_task,       // Function
    //     "coap_server_notify_task",     // Name
    //     4096,                   // Stack size
    //     NULL,                   // Parameters
    //     2,                      // Priority
    //     NULL                    // Handle
    // );
    
    ESP_LOGI(TAG, "All tasks created");
}

void loop(){}