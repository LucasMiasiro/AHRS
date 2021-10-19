#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "serial-logger.h"
#include "gpio-handler.h"
#include "main.h"
#include "config.h"
#include "GY87.h"
// #include "esp_dsp.h"

#if DEFAULT_MODE
extern "C" void app_main(void)
{
    static TaskHandle_t sensorTask_h = NULL, navTask_h = NULL;

    static GY87 IMU;
    static float A[3], G[3], M[3];
    static navData_ptr navData = {.IMU_ptr = &IMU,
                                .A_ptr = A,
                                .G_ptr = G,
                                .M_ptr = M};

    vTaskDelay(1000/portTICK_PERIOD_MS);
    #if LOG_MAIN
    log::header();
    #endif

    xTaskCreatePinnedToCore(sensorTask,
                            "Sensor Task",
                            1*1024,
                            &IMU,
                            1,
                            &sensorTask_h,
                            1);

    xTaskCreatePinnedToCore(navTask,
                            "Navigation Task",
                            1*8096,
                            &navData,
                            2,
                            &navTask_h,
                            1);

    // xTaskCreatePinnedToCore(navTask2,
    //                         "Navigation Task",
    //                         4096,
    //                         &IMU,
    //                         2,
    //                         &navTask_h,
    //                         1);

    
}

#elif CALIBRATE_MAG
extern "C" void app_main(void)
{
    float mag_param[6] = {0};
    uint8_t count = 0;
    builtin_led Led;

    GY87 IMU;
    while(1){
        if (IMU.calibrate_mag(&mag_param[0])){
            count = 0;
        }

        if (count < 10){
            Led.set_level(1);
            count++;
        } else {
            Led.set_level(0);
        }

        vTaskDelay(10);
    }
}
#else
extern "C" void app_main(void)
{
    #if LOG_MAIN
    log::header();
    #endif

    GY87 IMU;
    while(1){
        IMU.read_test();
        vTaskDelay(10);
    }
}
#endif

void sensorTask(void* Parameters){
    GY87* IMU = (GY87*) Parameters;
    while(1){
        IMU->accumulate_data();
        // std::cout << "newdata" << std::endl;
        vTaskDelay(10);
    }
}

void navTask(void* Parameters){
    navData_ptr* navData = (navData_ptr*) Parameters;
    while(1){
        navData->IMU_ptr->get_data(navData->A_ptr, navData->G_ptr, navData->M_ptr);
        std::cout << *(navData->A_ptr) << std::endl;
        vTaskDelay(100);
    }
}

void navTask2(void* Parameters){
    GY87* IMU = (GY87*) Parameters;
    float A[3], B[3], C[3];
    while(1){
        IMU->get_data(A,B,C);
        vTaskDelay(100);
    }
}