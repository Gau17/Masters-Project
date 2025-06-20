#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "servo_motor_controller.h"
#include "wifi_comms.h"
#include "coap_client.h"

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
    Serial.begin(115200);
    ESP_LOGI(TAG, "ESP32 Servo Controller Starting");

    // Initialize servo motor to 0 degrees
    servo_init(&servo_motor_1, 0); 

    // Initialize WiFi Communications
    ESP_LOGI(TAG, "Initializing WiFi Communications...");
    wifi_comms_init(MAIN_WIFI_SSID, MAIN_WIFI_PASSWORD);
    ESP_LOGI(TAG, "WiFi Communications Initialized.");
    
    // Create command queue for servo
    servo_cmd_queue = xQueueCreate(10, sizeof(Servo_cmd));
    if (servo_cmd_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create servo command queue");
        return;
    }

    coap_client_init();
    
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

    xTaskCreate(
        coap_client_task,       // Function
        "coap_client_task",     // Name
        4096,                   // Stack size
        NULL,                   // Parameters
        2,                      // Priority
        NULL                    // Handle
    );

    xTaskCreate(
        coap_get_request_test,
        "coap_get_request_test",
        4096,
        NULL,
        1,
        NULL
    );
    
    ESP_LOGI(TAG, "All tasks created");
}

void loop(){}