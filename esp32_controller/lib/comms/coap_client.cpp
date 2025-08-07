#include "coap_client.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include "cbor.h"

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

// Serialize Servo_cmd to CBOR binary
std::vector<uint8_t> serialize_servo_cmd_cbor(const Servo_cmd& cmd) {
    std::vector<uint8_t> buffer(128); // Initial buffer size
    CborEncoder encoder;
    CborEncoder mapEncoder;
    
    cbor_encoder_init(&encoder, buffer.data(), buffer.size(), 0);
    cbor_encoder_create_map(&encoder, &mapEncoder, 3); // 3 key-value pairs
    
    // Add angle
    cbor_encode_text_stringz(&mapEncoder, "angle");
    cbor_encode_uint(&mapEncoder, cmd.angle);
    
    // Add delay_ms
    cbor_encode_text_stringz(&mapEncoder, "delay_ms");
    cbor_encode_uint(&mapEncoder, cmd.delay_ms);
    
    // Add servo GPIO pin as identifier
    cbor_encode_text_stringz(&mapEncoder, "servo_gpio");
    cbor_encode_uint(&mapEncoder, cmd.servo->gpio_pin);
    
    cbor_encoder_close_container(&encoder, &mapEncoder);
    
    size_t encoded_size = cbor_encoder_get_buffer_size(&encoder, buffer.data());
    buffer.resize(encoded_size);
    
    return buffer;
}

// Send servo command as CBOR
void coap_send_servo_cmd_cbor(const char* endpoint, const Servo_cmd& cmd) {
    std::vector<uint8_t> cbor_payload = serialize_servo_cmd_cbor(cmd);
    Serial.printf("\nSending servo command (CBOR) to %s: %zu bytes\n", endpoint, cbor_payload.size());
    coapClient.Post(endpoint, cbor_payload, coap_client_process_payload);
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

// Test task for sending servo commands
void coap_servo_cmd_test_task(void *pvParameters) {

    static Servo test_servo = {
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
    
    while (true) {
        Servo_cmd cmd;
        cmd.angle = 90;
        cmd.delay_ms = 1000;
        cmd.servo = &test_servo;
        
        coap_send_servo_cmd_cbor("servo/cmd", cmd);
        
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}