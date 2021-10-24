#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "serial-logger.h"
#include "main.h"
#include "config.h"
#include "GY87.h"
#include "DCM.h"

#if DEFAULT_MODE
extern "C" void app_main(void)
{
    static TaskHandle_t sensorTask_h = NULL, navTask_h = NULL, sendTask_h = NULL;

    static GY87 IMU;
    static float A[3], G[3], M[3];
    static float eulerAngles[3], eulerAngRates[3];

    static navData_ptr navData = {.IMU_ptr = &IMU,
                                .A_ptr = A,
                                .G_ptr = G,
                                .M_ptr = M,
                                .eulerAngles_ptr = eulerAngles,
                                .eulerAngRates_ptr = eulerAngRates};

    vTaskDelay(1000/portTICK_PERIOD_MS);
    #if LOG_MAIN
    serialLogger::header();
    #endif

    xTaskCreatePinnedToCore(sensorTask,
                            "Sensor Task",
                            1*1024,
                            &IMU,
                            1,
                            &sensorTask_h,
                            1);

    vTaskDelay(SYSTEM_SAMPLE_PERIOD_MS/portTICK_PERIOD_MS);

    xTaskCreatePinnedToCore(navTask,
                            "Navigation Task",
                            4*1024,
                            &navData,
                            3,
                            &navTask_h,
                            1);

    xTaskCreatePinnedToCore(sendTask,
                            "Send Task",
                            4*1024,
                            &navData,
                            2,
                            &sendTask_h,
                            1);
}

#elif CALIBRATE_MAG
extern "C" void app_main(void)
{
    GY87 IMU;
    while(1){
        IMU.magCalibrationLoop();
        vTaskDelay(10);
    }
}
#elif CALIBRATE_GYRO
extern "C" void app_main(void)
{
    GY87 IMU;
    while(1){
        IMU.gyroCalibrationLoop();
        vTaskDelay(10);
    }
}
#else
extern "C" void app_main(void)
{
}
#endif

void sensorTask(void* Parameters){
    GY87* IMU = (GY87*) Parameters;

    while(1){
        IMU->accumulateData();
    }
}

void navTask(void* Parameters){
    navData_ptr* navData = (navData_ptr*) Parameters;
    DCM DCM;
    DCM.initializeFilter();

#if LOG_TIMER
    int64_t start = esp_timer_get_time();
    int64_t dt = 0;
#endif

    TickType_t startTimer = xTaskGetTickCount();

    while(1){
        navData->IMU_ptr->getData(navData->A_ptr, navData->G_ptr, navData->M_ptr);
        DCM.update(navData->A_ptr, navData->G_ptr, navData->M_ptr);
        DCM.getStates(navData->eulerAngles_ptr, navData->eulerAngRates_ptr);

        vTaskDelayUntil(&startTimer, SYSTEM_SAMPLE_PERIOD_MS/portTICK_PERIOD_MS);

#if LOG_TIMER
        dt = esp_timer_get_time() - start;
        start = esp_timer_get_time();
        serialLogger::logInt64(&dt, "DTNAV");
#endif

    }
}

void sendTask(void* Parameters){
    navData_ptr* navData = (navData_ptr*) Parameters;
    TickType_t startTimer = xTaskGetTickCount();

#if LOG_TIMER
    int64_t start = esp_timer_get_time();
    int64_t dt = 0;
#endif

    while(1){

#if SEND_SERIAL
        serialLogger::logFloat(navData->eulerAngles_ptr, 3, 1/DEG2RAD, "ATT");
        serialLogger::logFloat(navData->A_ptr, 3, "ACCEL");
        serialLogger::logFloat(navData->G_ptr, 3, "GYRO");
        serialLogger::logFloat(navData->M_ptr, 3, "MAG");
        serialLogger::logFloat(&navData->IMU_ptr->magModule, 1, "MMOD");
        serialLogger::blank_lines(1);
#endif

        vTaskDelayUntil(&startTimer, SYSTEM_SAMPLE_PERIOD_MS/portTICK_PERIOD_MS);

#if LOG_TIMER
        dt = esp_timer_get_time() - start;
        start = esp_timer_get_time();
        serialLogger::logInt64(&dt, "DTSEND");
#endif

    }
}
