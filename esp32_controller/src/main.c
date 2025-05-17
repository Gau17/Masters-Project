#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    printf("Hello from ESP32!\n");
    // Your ESP32 application code here
    while(1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
