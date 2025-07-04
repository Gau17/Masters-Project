#include "coap_client.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>

IPAddress COAP_SERVER_IP = IPAddress(10,0,0,244);
const uint16_t COAP_SERVER_PORT = 5683;

Thing::CoAP::Client coapClient;
Thing::CoAP::ESP::UDPPacketProvider udpProvider;

// static const char *TAG = "COAP_CLIENT";

void coap_client_init()
{
    coapClient.SetPacketProvider(udpProvider);
    coapClient.Start(COAP_SERVER_IP, COAP_SERVER_PORT);
    Serial.println("Started Coap Client");
}

void coap_client_process_payload(Thing::CoAP::Response response)
{
    int code = static_cast<int>(response.GetCode());
    int response_class = code / 32;
    int response_detail = code % 32;

    Serial.printf("\n--- Server Response ---\n");
    Serial.printf("Response Code: %d.%02d\n", response_class, response_detail);

    // Only process payload for success codes (class 2)
    if (response_class == 2) {
        std::vector<uint8_t> payload = response.GetPayload();
        if (!payload.empty()) {
            std::string received(payload.begin(), payload.end());
            Serial.println("Message: ");
            Serial.print(received.c_str());
        } else {
            Serial.println("Message: (No payload received)");
        }
    } else {
        Serial.println("An error occurred. No payload to process.");
    }
}

void coap_get_request()
{
    coapClient.Get("sensor/data", "", coap_client_process_payload);
}

void coap_post_request_binary(const char* endpoint, const std::vector<uint8_t>& binary_data)
{
    Serial.printf("\nSending POST request to endpoint: %s with %zu bytes of binary data", 
             endpoint, binary_data.size());
    
    // Create a payload from the binary data
    std::string payload(binary_data.begin(), binary_data.end());
    // Send POST request with binary data
    coapClient.Post(endpoint, payload, coap_client_process_payload);
}

void coap_post_request_text(const char* endpoint, const std::string& text_data)
{
    Serial.printf("\nSending POST request to endpoint: %s with text data: %s", 
             endpoint, text_data.c_str());
    
    // Send POST request with text data
    coapClient.Post(endpoint, text_data, coap_client_process_payload);
}

void coap_client_task(void *pvParameters) {
    while (true) {
        coapClient.Process();   // Handle incoming/outgoing CoAP packets
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void coap_get_request_test(void *pvParameters){
    while (true)
    {
        coap_get_request();
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
    
}

void coap_post_request_test(void *pvParameters){
    while (true)
    {
        // Example: Send binary data (e.g., sensor readings)
        std::vector<uint8_t> sensor_data = {
            0x01, 0x02, 0x03, 0x04,  // Example sensor values
            0x05, 0x06, 0x07, 0x08
        };
        
        coap_post_request_binary("sensor/data", sensor_data);
        
        // // Example: Send text data
        // std::string status_message = "Device status: OK";
        // coap_post_request_text("basic", status_message);
        
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}