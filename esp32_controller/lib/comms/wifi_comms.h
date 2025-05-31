#ifndef WIFI_COMMS_H
#define WIFI_COMMS_H

#include <WiFi.h>
#include <ArduinoJson.h>
#include <stdint.h> 
#include "freertos/queue.h"

const char* MAIN_WIFI_SSID = "YOUR_WIFI_SSID";
const char* MAIN_WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* MAIN_SERVER_IP = "YOUR_RPI_SERVER_IP";
const int MAIN_SERVER_PORT = 12345;

typedef struct {
    int id;
    int angle;
} ServoData_t;

typedef struct {
    uint32_t timestamp;
    ServoData_t servos[6];
    int numServos;
} CommPacket_t;

extern const char* MAIN_WIFI_SSID;
extern const char* MAIN_WIFI_PASSWORD;
extern const char* MAIN_SERVER_IP;
extern const int MAIN_SERVER_PORT;

// Internal module state variables (already externed, to be set by wifi_comms_init)
// extern const char* ssid; // These are actually non-const char* in the .cpp, and represent current operational values
// extern const char* password;
// extern const char* serverIp;
// extern int serverPort;

// Queue for communication packets
extern QueueHandle_t commQueue;
#define COMM_QUEUE_LENGTH 10 // Max number of packets in the queue
#define COMM_PACKET_SEND_INTERVAL_MS 1000 // Send data every 1 second

// Function declarations
void wifi_comms_init(const char* wifi_ssid, const char* wifi_password, const char* rpi_server_ip, int rpi_server_port);
bool wifi_comms_add_packet_to_queue(const CommPacket_t* packet);
void wifi_comms_task(void *pvParameters);
void send_dummy_data_task(void *pvParameters);

#endif
