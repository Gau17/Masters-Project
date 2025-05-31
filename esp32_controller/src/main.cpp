// Working code with freertos and mcpwm
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "servo_motor_controller.h"
#include "wifi_comms.h"

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

void send_dummy_data_task(void *pvParameters) {
        CommPacket_t dummy_packet;
        dummy_packet.numServos = 1; 
        ESP_LOGI(TAG, "send_dummy_data_task started");

        for (;;) {
            dummy_packet.timestamp = esp_log_timestamp(); 
            
            // Critical section: Accessing shared servo_motor_1.current_angle
            // This should ideally be thread-safe if current_angle is modified by another task.
            dummy_packet.servos[0].id = servo_motor_1.gpio_pin; 
            dummy_packet.servos[0].angle = servo_motor_1.current_angle; 

            ESP_LOGI(TAG, "[DummyTask] Q Pre: Servo ID %d, Angle %d", 
                        dummy_packet.servos[0].id, 
                        dummy_packet.servos[0].angle);

            if (wifi_comms_add_packet_to_queue(&dummy_packet)) {
                ESP_LOGI(TAG, "[DummyTask] Packet added to queue.");
            } else {
                ESP_LOGW(TAG, "[DummyTask] Failed to add packet. Queue full?");
            }

            vTaskDelay(pdMS_TO_TICKS(5000)); 
        }
    }

// Application main entry point
void setup(void) {
    ESP_LOGI(TAG, "ESP32 Servo Controller Starting");

    // Initialize servo motor to 0 degrees
    servo_init(&servo_motor_1, 0); 

    // Initialize WiFi Communications
    ESP_LOGI(TAG, "Initializing WiFi Communications...");
    wifi_comms_init(MAIN_WIFI_SSID, MAIN_WIFI_PASSWORD, MAIN_SERVER_IP, MAIN_SERVER_PORT);
    ESP_LOGI(TAG, "WiFi Communications Initialized.");
    
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

    xTaskCreate(
        send_dummy_data_task,  // Function
        "dummy_data_sender",   // Name
        4096,                  // Stack size
        NULL,                  // Parameters
        2,                     // Priority
        NULL                   // Handle
    );
    ESP_LOGI(TAG, "Dummy data sender task created.");
    
    ESP_LOGI(TAG, "All tasks created");
}

void loop(){}