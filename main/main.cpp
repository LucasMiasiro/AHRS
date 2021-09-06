#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "serial-logger.h"
#include "gpio-handler.h"
#include "main.h"
#include "config.h"
#include "GY87.h"
// #include "esp_dsp.h"

#if CALIBRATE_MAG == 0
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

#elif CALIBRATE_MAG == 1
extern "C" void app_main(void)
{
    float mag_param[6] = {0};
    const uint8_t nmax = 10;
    uint8_t count = 0;
    bool newMax = 0;
    builtin_led Led;

    GY87 IMU;
    IMU.setup();
    while(1){
        newMax = IMU.calibrate_mag(&mag_param[0]);
        if (newMax){
            count = 0;
        }

        if (count < nmax){
            Led.set_level(1);
            count++;
        } else {
            Led.set_level(0);
        }

        vTaskDelay(10);
    }
}
#endif