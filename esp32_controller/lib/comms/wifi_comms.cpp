#include "wifi_comms.h"
#include <WiFi.h>
#include "esp_log.h"      
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Data hidden for commit purposes
const char* MAIN_WIFI_SSID = "abcd";
const char* MAIN_WIFI_PASSWORD = "abcd";

// const char *TAG = "wifi_comms";


void wifi_comms_init(const char* wifi_ssid_param, const char* wifi_password_param) {

    Serial.printf("Initializing WiFi with SSID: %s", wifi_ssid_param);
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid_param, wifi_password_param);
    Serial.printf("Connecting to WiFi network: %s", wifi_ssid_param);
    int connect_tries = 0;
    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(pdMS_TO_TICKS(500));
        connect_tries++;
        if (connect_tries % 10 == 0) {
            Serial.printf( ".");
        }
        if (connect_tries > 40) {
            Serial.printf("Failed to connect to WiFi!");
            return; 
        }
    }
    Serial.printf( "Connected to WiFi. IP Address: %s", WiFi.localIP().toString().c_str());
}