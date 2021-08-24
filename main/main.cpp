#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "serial-logger.h"
#include "main.h"
#include "config.h"
#include "GY87.h"
// #include "esp_dsp.h"

extern "C" void app_main(void)
{
    #if LOG_MAIN
    log::header();
    #endif

    GY87 IMU;
    IMU.setup();
    while(1){
        IMU.read_test();
        vTaskDelay(10);
    }
}
