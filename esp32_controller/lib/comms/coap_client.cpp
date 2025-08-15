#include "coap_client.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include "cbor.h"
#include "esp_timer.h"
#include <sys/time.h>
#include "sntp.h"

IPAddress COAP_SERVER_IP = IPAddress(10,0,0,244);
const uint16_t COAP_SERVER_PORT = 5683;

Thing::CoAP::Client coapClient;
Thing::CoAP::ESP::UDPPacketProvider udpProvider;

// static const char *TAG = "COAP_CLIENT";

int64_t g_startTime = 0;

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

// Decode CBOR payload (map with 6 keys) into RobotPose
bool decode_robot_pose_cbor(const std::vector<uint8_t>& buf, RobotPose& out, int64_t* server_timestamp = nullptr){
    CborParser parser;
    CborValue it;
    if(cbor_parser_init(buf.data(), buf.size(), 0, &parser, &it) != CborNoError) return false;
    if(!cbor_value_is_map(&it)) return false;
    
    CborValue mapIt;
    if(cbor_value_enter_container(&it, &mapIt) != CborNoError) return false;

    // Initialize to defaults
    out = {0,0,0,0,0,0};
    if(server_timestamp) *server_timestamp = 0;
    
    // Process each key-value pair
    while(!cbor_value_at_end(&mapIt)){
        // Read key
        if(!cbor_value_is_text_string(&mapIt)) return false;
        char key[32]; 
        size_t keyLen = sizeof(key);
        if(cbor_value_copy_text_string(&mapIt, key, &keyLen, &mapIt) != CborNoError) return false;
        
        // Read value
        if(!cbor_value_is_unsigned_integer(&mapIt)) return false;
        uint64_t value;
        if(cbor_value_get_uint64(&mapIt, &value) != CborNoError) return false;
        
        // Map to correct field
        if(strcmp(key, "waist") == 0)           out.waist_angle = (uint32_t)value;
        else if(strcmp(key, "shoulder") == 0)   out.shoulder_angle = (uint32_t)value;
        else if(strcmp(key, "elbow") == 0)      out.elbow_angle = (uint32_t)value;
        else if(strcmp(key, "wrist_pitch") == 0) out.wrist_pitch_angle = (uint32_t)value;
        else if(strcmp(key, "wrist_roll") == 0) out.wrist_roll_angle = (uint32_t)value;
        else if(strcmp(key, "gripper") == 0)    out.gripper_angle = (uint32_t)value;
        else if(strcmp(key, "server_timestamp") == 0 && server_timestamp) *server_timestamp = (int64_t)value;
        // else: unknown key, ignore
        
        // Advance to next key-value pair
        if(cbor_value_advance(&mapIt) != CborNoError) return false;
    }
    return true;
}

// static void coap_pose_observe_callback(Thing::CoAP::Response resp){
//     int code = static_cast<int>(resp.GetCode());
//     int cls = code / 32;
//     if(cls != 2){
//         Serial.println("Pose observe: non-success response");
//         return;
//     }
//     auto payload = resp.GetPayload();
//     if(payload.empty()){
//         Serial.println("Pose observe: empty payload");
//         return;
//     }
//     RobotPose pose;
//     if(!decode_robot_pose_cbor(payload, pose)){
//         Serial.println("Pose observe: CBOR decode failed");
//         return;
//     }
//     Serial.printf("Pose received: waist=%u shoulder=%u elbow=%u wrist_pitch=%u wrist_roll=%u gripper=%u\n",
//         pose.waist_angle, pose.shoulder_angle, pose.elbow_angle,
//         pose.wrist_pitch_angle, pose.wrist_roll_angle, pose.gripper_angle);
//     apply_robot_pose(pose);
// }

// void coap_start_pose_observe(){
//     // Requires server to support Observe on /arm/pose
//     coapClient.Observe("arm/pose", coap_pose_observe_callback);
// }

// Modified pose poll callback with end-to-end timing
static void pose_poll_callback(Thing::CoAP::Response resp) {

    struct timeval tv_end;
    gettimeofday(&tv_end, NULL);
    int64_t endTime = (int64_t)tv_end.tv_sec * 1000000L + tv_end.tv_usec;

    // Calculate the difference from the global start time.
    int64_t rtt_us = endTime - g_startTime;

    Serial.printf("[RTT] Round-Trip Time: %" PRId64 " us (%.2f ms)\n",
                  rtt_us, rtt_us / 1000.0);

    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t esp32_unix_time_us = (int64_t)tv.tv_sec * 1000000 + tv.tv_usec;

    int64_t coap_rx_time = esp_timer_get_time();
    
    int code = static_cast<int>(resp.GetCode());
    int cls  = code / 32;
    if (cls != 2) {
        Serial.printf("[POSE-POLL] Non-success code %d.%02d\n", cls, code % 32);
        return;
    }
    auto payload = resp.GetPayload();
    if (payload.empty()) {
        Serial.println("[POSE-POLL] Empty payload");
        return;
    }
    
    RobotPose pose;
    int64_t server_timestamp_us = 0;
    if (!decode_robot_pose_cbor(payload, pose, &server_timestamp_us)) {
        Serial.println("[POSE-POLL] CBOR decode failed");
        return;
    }
    
    int64_t decode_time = esp_timer_get_time();
    Serial.printf("[TIMING] CoAP RX->Decode: %lld us\n", decode_time - coap_rx_time);
    
    // Only calculate network delay if we have NTP sync and server timestamp
    if (server_timestamp_us > 0) {
        int64_t network_delay_us = esp32_unix_time_us + (rtt_us/2) - server_timestamp_us;
        Serial.printf("[TIMING] Network Delay: %lld us (%.2f ms)\n", 
                      network_delay_us, network_delay_us / 1000.0);
        
        // // Debug: show both timestamps
        // Serial.printf("[DEBUG] Server: %lld us, ESP32: %lld us\n", 
        //               server_timestamp_us, esp32_unix_time_us);
    } else {
        Serial.println("[TIMING] Network delay calculation skipped (no NTP sync or server timestamp)");
    }
    
    Serial.printf("[POSE-POLL] waist=%u shoulder=%u elbow=%u wrist_pitch=%u wrist_roll=%u gripper=%u\n",
                  pose.waist_angle, pose.shoulder_angle, pose.elbow_angle,
                  pose.wrist_pitch_angle, pose.wrist_roll_angle, pose.gripper_angle);
    
    apply_robot_pose_with_timing(pose, coap_rx_time);
}

// Send one poll request
void coap_poll_pose_once() {
    struct timeval tv_start;
    gettimeofday(&tv_start, NULL);
    g_startTime = (int64_t)tv_start.tv_sec * 1000000L + tv_start.tv_usec;

    coapClient.Get("arm/pose", "", pose_poll_callback);
}

// Task for periodic polling
void coap_pose_poll_task(void *pv) {
    const uint32_t interval_ms = 3000; // adjust as needed
    Serial.println("[POSE-POLL] Poll task started");
    while (true) {
        coap_poll_pose_once();
        vTaskDelay(pdMS_TO_TICKS(interval_ms));
    }
}

void sync_time_with_ntp() {
    Serial.println("Syncing time with NTP...");
    configTime(0, 0, "pool.ntp.org");  // GMT offset=0, DST offset=0
    
    struct tm timeinfo;
    int retry = 0;
    const int retry_count = 10;
    while (!getLocalTime(&timeinfo) && ++retry < retry_count) {
        Serial.print(".");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    
    if (retry < retry_count) {
        Serial.println("\nTime synchronized with NTP");
        Serial.printf("Current time: %04d-%02d-%02d %02d:%02d:%02d\n",
                     timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                     timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        
        // Also show Unix timestamp for verification
        time_t now;
        time(&now);
        Serial.printf("Unix timestamp: %ld\n", now);
    } else {
        Serial.println("\nFailed to sync time with NTP");
    }
}