#include "coap_server.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>

Thing::CoAP::Server coapServer;
Thing::CoAP::ESP::UDPPacketProvider udpProvider;

Thing::CoAP::IFunctionalEndpoint* basic_endpoint = nullptr;


void coap_server_init(){
    coapServer.SetPacketProvider(udpProvider);

    basic_endpoint = &coapServer.CreateEndpoint("basic", Thing::CoAP::ContentFormat::TextPlain, true).OnGet([](Thing::CoAP::Request & request){
        Serial.println("GET Request received for endpoint 'basic'");
        return Thing::CoAP::Status::Content("Hello from Server");
    }).OnPost([](Thing::CoAP::Request & request){
        Serial.println("POST Request received for enpoint 'basic'");
        std::vector<uint8_t> payload = request.GetPayload();
        if (!payload.empty()) {
        Serial.printf("Received payload with %zu bytes:\n", payload.size());
        
        // Print as text (if it's text data)
        std::string text_payload(payload.begin(), payload.end());
        Serial.printf("Text: %s\n", text_payload.c_str());
        
        // Print as hex (useful for binary data)
        Serial.print("Hex: ");
        for (size_t i = 0; i < payload.size(); i++) {
            Serial.printf("%02X ", payload[i]);
        }
        Serial.println();
    } else {
        Serial.println("No payload received");
    }
    
    return Thing::CoAP::Status::Created("POST received");
    });

    coapServer.Start();

    Serial.println("Started Coap Server");
}

void coap_server_task(void *pvParameters) {
    while (true) {
        coapServer.Process();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void coap_basic_notify_task(void *pvParameters) {
    while (true) {
        if (basic_endpoint) {
            Thing::CoAP::Status status = Thing::CoAP::Status::Ok("Periodic update from server");
            basic_endpoint->ObservableChanged(status);
            Serial.println("Notified observers of 'basic' endpoint");
        }
        vTaskDelay(pdMS_TO_TICKS(5000)); // 5 seconds
    }
}
