#include "wifi_comms.h"
#include <ArduinoJson.h>
#include "esp_log.h"      
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h" // For esp_log_timestamp, if used directly

// Potentially for String class if not pulled in by WiFi.h or ArduinoJson.h for some reason
// #include <WString.h> 

const char *TAG = "wifi_comms";

// Internal module state variables (not const, as they are assigned in _init)
const char* ssid_internal = nullptr; 
const char* password_internal = nullptr;
const char* serverIp_internal = nullptr;
int serverPort_internal = 0;

QueueHandle_t commQueue = nullptr;

/**
 * @brief Initializes WiFi connection and creates the communication queue.
 *
 * @param wifi_ssid SSID of the WiFi network.
 * @param wifi_password Password for the WiFi network.
 * @param rpi_server_ip IP address of the Raspberry Pi server.
 * @param rpi_server_port Port number of the Raspberry Pi server.
 */
void wifi_comms_init(const char* wifi_ssid_param, const char* wifi_password_param, const char* rpi_server_ip_param, int rpi_server_port_param) {
    ssid_internal = wifi_ssid_param;
    password_internal = wifi_password_param;
    serverIp_internal = rpi_server_ip_param;
    serverPort_internal = rpi_server_port_param;

    ESP_LOGI(TAG, "Initializing WiFi with SSID: %s", ssid_internal);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid_internal, password_internal);
    ESP_LOGI(TAG, "Connecting to WiFi network: %s", ssid_internal);
    int connect_tries = 0;
    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(pdMS_TO_TICKS(500));
        connect_tries++;
        if (connect_tries % 10 == 0) {
            ESP_LOGI(TAG, ".");
        }
        if (connect_tries > 40) {
            ESP_LOGE(TAG, "Failed to connect to WiFi!");
            return; 
        }
    }
    ESP_LOGI(TAG, "Connected to WiFi. IP Address: %s", WiFi.localIP().toString().c_str());

    commQueue = xQueueCreate(COMM_QUEUE_LENGTH, sizeof(CommPacket_t));
    if (commQueue == nullptr) {
        ESP_LOGE(TAG, "Error creating the comm queue");
    } else {
        ESP_LOGI(TAG, "Comm queue created successfully.");
    }

    BaseType_t task_created = xTaskCreate(
        wifi_comms_task,    
        "WiFiCommTask",     
        4096,
        NULL,               
        1,
        NULL);              
    if (task_created != pdPASS) {
        ESP_LOGE(TAG, "Failed to create WiFiCommTask");
    } else {
        ESP_LOGI(TAG, "WiFiCommTask created.");
    }
}

/**
 * @brief Adds a communication packet to the sending queue.
 *
 * @param packet Pointer to the CommPacket_t to be sent.
 * @return true if the packet was successfully added to the queue, false otherwise.
 */
bool wifi_comms_add_packet_to_queue(const CommPacket_t* packet) {
    if (commQueue == nullptr) {
        ESP_LOGE(TAG, "Comms queue not initialized! Cannot add packet.");
        return false;
    }
    if (xQueueSend(commQueue, packet, (TickType_t)0) != pdPASS) { // Non-blocking send
        ESP_LOGW(TAG, "Failed to send packet to commQueue. Queue might be full.");
        return false;
    }
    return true;
}

/**
 * @brief FreeRTOS task that periodically checks the queue and sends data.
 *
 * @param pvParameters Task parameters (not used).
 */
void wifi_comms_task(void *pvParameters) {
    CommPacket_t packet_to_send;
    WiFiClient client; 

    ESP_LOGI(TAG, "wifi_comms_task started");

    for (;;) { 
        if (xQueueReceive(commQueue, &packet_to_send, pdMS_TO_TICKS(COMM_PACKET_SEND_INTERVAL_MS))) {
            if (WiFi.status() != WL_CONNECTED) {
                ESP_LOGW(TAG, "WiFi not connected. Holding packet.");
                vTaskDelay(pdMS_TO_TICKS(100));
                if (!wifi_comms_add_packet_to_queue(&packet_to_send)) {
                     ESP_LOGE(TAG, "Failed to re-queue packet after WiFi disconnect. Packet lost.");
                }
                continue;
            }

            ESP_LOGI(TAG, "Attempting to connect to server: %s:%d", serverIp_internal, serverPort_internal);
            if (client.connect(serverIp_internal, serverPort_internal)) {
                ESP_LOGI(TAG, "Connected to server. Sending packet...");

                DynamicJsonDocument jsonDoc(1024); 
                jsonDoc["timestamp"] = packet_to_send.timestamp;
                JsonArray servoArray = jsonDoc.createNestedArray("servos");
                for (int i = 0; i < packet_to_send.numServos; ++i) {
                    JsonObject servoObj = servoArray.createNestedObject();
                    servoObj["id"] = packet_to_send.servos[i].id;
                    servoObj["angle"] = packet_to_send.servos[i].angle;
                }
                
                String jsonString;
                serializeJson(jsonDoc, jsonString);

                size_t sent_bytes = client.print(jsonString);
                if (sent_bytes == jsonString.length()){
                    ESP_LOGI(TAG, "Sent %d bytes: %s", sent_bytes, jsonString.c_str());
                } else {
                    ESP_LOGE(TAG, "Failed to send complete JSON packet. Sent %d of %d bytes.", sent_bytes, jsonString.length());
                }
                client.stop(); 
                ESP_LOGI(TAG, "Connection closed.");
            } else {
                ESP_LOGE(TAG, "Connection to server %s:%d failed.", serverIp_internal, serverPort_internal);
                ESP_LOGI(TAG, "Re-queuing packet due to connection failure.");
                if (!wifi_comms_add_packet_to_queue(&packet_to_send)) {
                     ESP_LOGE(TAG, "Failed to re-queue packet after server connection failure. Packet lost.");
                }
            }
        } 
    }
}