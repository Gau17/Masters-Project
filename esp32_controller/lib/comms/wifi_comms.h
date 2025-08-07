#ifndef WIFI_COMMS_H
#define WIFI_COMMS_H

extern const char* MAIN_WIFI_SSID;
extern const char* MAIN_WIFI_PASSWORD;

/**
 * @brief Initializes WiFi connection.
 *
 * @param wifi_ssid SSID of the WiFi network.
 * @param wifi_password Password for the WiFi network.
 */
void wifi_comms_init(const char* wifi_ssid, const char* wifi_password);

#endif
