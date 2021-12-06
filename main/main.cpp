#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio-handler.h"
#include "serial-logger.h"
#include "main.h"
#include "config.h"
#include "GY87.h"
#include "DCM.h"
#include "KF.h"
#include "NEO-M8N.h"

#include "BT.cpp"
#include "SD-SPI.cpp"

#if APP_MODE==0
extern "C" void app_main(void)
{
    #if LOG_MAIN
    serialLogger::header();
    #endif

    static TaskHandle_t sensorTask_h = NULL, navTask_h = NULL, sendTask_h = NULL,
                        gyroCalTask_h = NULL, magCalTask_h = NULL;

    static GY87 IMU;
    static float A[3], G[3], M[3], B;
    static float eulerAngles[3], eulerAngRates[3];
    static float pos[3], vel[3];
    static NEOM8N GNSS;
    builtin_led led;
    GNSS.initialize();

#if LOG_SD
    if(SD::startSD()) {
        led.set_level(0);
    } else {
        led.set_level(1);
    }
#endif

    GNSS.setHome();
    IMU.setHome();
    led.blink(10, false);

#if LOG_SD
    SD::writeHeader(&GNSS);
#endif

    static navData_ptr navData = {.IMU_ptr = &IMU,
                                .GNSS_ptr = &GNSS,
                                .A_ptr = A,
                                .G_ptr = G,
                                .M_ptr = M,
                                .B_ptr = &B,
                                .eulerAngles_ptr = eulerAngles,
                                .eulerAngRates_ptr = eulerAngRates,
                                .pos_ptr = pos,
                                .vel_ptr = vel};

    vTaskDelay(1000/portTICK_PERIOD_MS);

    xTaskCreatePinnedToCore(sensorTask,
                            "Sensor Task",
                            2*1024,
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

#if SEND_MODE==1

    xTaskCreatePinnedToCore(sendTask,
                            "Send Task",
                            4*1024,
                            &navData,
                            2,
                            &sendTask_h,
                            1); //TODO mudar para Core 0?

#elif SEND_MODE==2

    static serialBTLogger::navDataBT_ptr navDataBT = {.navData = &navData,
                                                    .sendTask_h = &sendTask_h,
                                                    .navTask_h = &navTask_h,
                                                    .sensorTask_h = &sensorTask_h,
                                                    .gyroCalTask_h = &gyroCalTask_h,
                                                    .magCalTask_h = &magCalTask_h};

    serialBTLogger::startBT(&navDataBT);

#endif

}

#elif APP_MODE==1

extern "C" void app_main(void)
{
    static GY87 IMU;
    static float A[3], G[3], M[3], B;
    static float eulerAngles[3], eulerAngRates[3];

    static navData_ptr navData = {.IMU_ptr = &IMU,
                                .GNSS_ptr = NULL,
                                .A_ptr = A,
                                .G_ptr = G,
                                .M_ptr = M,
                                .B_ptr = &B,
                                .eulerAngles_ptr = eulerAngles,
                                .eulerAngRates_ptr = eulerAngRates,
                                .pos_ptr = NULL,
                                .vel_ptr = NULL};

    magCalTask(&navData);
}

#elif APP_MODE==2
extern "C" void app_main(void)
{
    static GY87 IMU;
    static float A[3], G[3], M[3], B;
    static float eulerAngles[3], eulerAngRates[3];

    static navData_ptr navData = {.IMU_ptr = &IMU,
                                .GNSS_ptr = NULL,
                                .A_ptr = A,
                                .G_ptr = G,
                                .M_ptr = M,
                                .B_ptr = &B,
                                .eulerAngles_ptr = eulerAngles,
                                .eulerAngRates_ptr = eulerAngRates,
                                .pos_ptr = NULL,
                                .vel_ptr = NULL};

    gyroCalTask(&navData);
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
    float A_E[4] = {0.0f};
    DCM DCM;
    DCM.initializeFilter();
    KF KF(&DCM, navData);

#if LOG_TIMER
    int64_t start = esp_timer_get_time();
    int64_t dt = 0;
#endif

    TickType_t startTimer = xTaskGetTickCount();

    while(1){
        navData->IMU_ptr->getData(navData->A_ptr, navData->G_ptr,
                                  navData->M_ptr, navData->B_ptr);
        DCM.update(navData->A_ptr, navData->G_ptr, navData->M_ptr);
        DCM.getStates(navData->eulerAngles_ptr, navData->eulerAngRates_ptr);

#if AXIS_CONFIG==0
        A_E[1] = navData->A_ptr[1];
        A_E[2] = navData->A_ptr[0];
        A_E[3] = navData->A_ptr[2];
#elif AXIS_CONFIG==1
        A_E[1] = -(navData->A_ptr[1]);
        A_E[2] = -(navData->A_ptr[0]);
        A_E[3] = navData->A_ptr[2];
#endif

        DCM.rotate2Earth(A_E);

        KF.update(A_E);
        KF.getStates(navData->pos_ptr, navData->vel_ptr);

        vTaskDelayUntil(&startTimer, SYSTEM_SAMPLE_PERIOD_MS/portTICK_PERIOD_MS);

#if LOG_TIMER
        dt = esp_timer_get_time() - start;
        start = esp_timer_get_time();
        serialLogger::logInt64(&dt, "DTNAV");
#endif

    }
}

#if SEND_MODE==1
void sendTask(void* Parameters){
    navData_ptr* navData = (navData_ptr*) Parameters;
    TickType_t startTimer = xTaskGetTickCount();

#if LOG_TIMER
    int64_t start = esp_timer_get_time();
    int64_t dt = 0;
#endif

#if LOG_SD
    float timeData[3] = {0, 0, 0};
    const uint8_t nSave = SD_WRITE_UNTIL_SAVE;
    uint8_t count = 0;
    float *logSDData_ptr[] = {timeData,
                            navData->eulerAngles_ptr,
                            navData->pos_ptr,
                            navData->vel_ptr};
#endif

    while(1){

        serialLogger::logFloat(navData->eulerAngles_ptr, 3, 1/DEG2RAD, "ATT");
        serialLogger::logFloat(navData->A_ptr, 3, "ACCEL");
        serialLogger::logFloat(navData->G_ptr, 3, "GYRO");
        serialLogger::logFloat(navData->M_ptr, 3, "MAG");
        serialLogger::logFloat(navData->B_ptr, 1, "BARO");
        serialLogger::logFloat(&navData->IMU_ptr->magModule, 1, "MMOD");
        serialLogger::logFloat(navData->pos_ptr, 3, "POS");
        serialLogger::logFloat(navData->vel_ptr, 3, "VEL");
        serialLogger::blank_lines(1);

#if LOG_SD
        timeData[0] = esp_timer_get_time()/1000.0f;
        navData->GNSS_ptr->getPos(&timeData[1], &timeData[2]);
        SD::write(logSDData_ptr, 3, 4);
        count++;
        if (count > nSave){
            SD::save();
            count = 0;
        }
#endif

        vTaskDelayUntil(&startTimer, SYSTEM_SAMPLE_PERIOD_MS/portTICK_PERIOD_MS);

#if LOG_TIMER
        dt = esp_timer_get_time() - start;
        start = esp_timer_get_time();
        serialLogger::logInt64(&dt, "DTSEND");
#endif

    }
}
#endif

void gyroCalTask(void* Parameters){
    navData_ptr* navData = (navData_ptr*) Parameters;
    while(1){
        navData->IMU_ptr->gyroCalibrationLoop();
    }
}

void magCalTask(void* Parameters){
    navData_ptr* navData = (navData_ptr*) Parameters;
    while(1){
        navData->IMU_ptr->magCalibrationLoop();
    }
}
