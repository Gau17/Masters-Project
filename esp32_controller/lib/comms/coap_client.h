#ifndef COAP_CLIENT_H
#define COAP_CLIENT_H

#include "Thing.CoAP.h"

extern IPAddress COAP_SERVER_IP;
extern const uint16_t COAP_SERVER_PORT;

extern Thing::CoAP::Client coapClient;
extern Thing::CoAP::ESP::UDPPacketProvider udpProvider;

// Initializes WiFi and CoAP client
void coap_client_init();

// Sends a GET request to the given endpoint
void coap_get_request();

// FreeRTOS task to process CoAP client events
void coap_client_task(void *pvParameters);

// FreeRTOS task to test CoAP get request
void coap_get_request_test(void *pvParameters);

// Sends a post request with binary payload
void coap_post_request_binary(const char* endpoint, const std::vector<uint8_t>& binary_data);

// FreeRTOS task to test CoAP post request
void coap_post_request_test(void *pvParameters);

#endif