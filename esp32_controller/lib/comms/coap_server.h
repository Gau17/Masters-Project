#ifndef COAP_SERVER_H
#define COAP_SERVER_H

#include "Thing.CoAP.h"

extern Thing::CoAP::Server coapServer;
extern Thing::CoAP::ESP::UDPPacketProvider udpProvider;

void coap_server_init();

void coap_server_task(void *pvParameters);

void coap_basic_notify_task(void *pvParameters);

#endif