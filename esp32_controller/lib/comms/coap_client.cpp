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
    std::vector<uint8_t> payload = response.GetPayload();
    std::string received(payload.begin(), payload.end());
    Serial.println("Server sent the following message:");
    Serial.println(received.c_str());
}

void coap_get_request()
{
    coapClient.Get("basic", "", coap_client_process_payload);
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