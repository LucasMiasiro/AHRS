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
    while(1){
        navData->IMU_ptr->getData(navData->A_ptr, navData->G_ptr, navData->M_ptr);
        DCM.update(navData->A_ptr, navData->G_ptr, navData->M_ptr);
        DCM.getStates(navData->eulerAngles_ptr, navData->eulerAngRates_ptr);
        vTaskDelay(SYSTEM_SAMPLE_PERIOD_MS/portTICK_PERIOD_MS); //TODO: Setar taxa fixa para execução da Navegação
    }
}

void sendTask(void* Parameters){
    navData_ptr* navData = (navData_ptr*) Parameters;
    while(1){

        std::cout << "ATT ";
        std::cout << *(navData->eulerAngles_ptr)/DEG2RAD << " ";
        std::cout << *(navData->eulerAngles_ptr+1)/DEG2RAD << " ";
        std::cout << *(navData->eulerAngles_ptr+2)/DEG2RAD << " ";
        std::cout << std::endl;

        std::cout << "A ";
        std::cout << *(navData->A_ptr) << " ";
        std::cout << *(navData->A_ptr+1) << " ";
        std::cout << *(navData->A_ptr+2) << " ";
        std::cout << std::endl;

        std::cout << "G ";
        std::cout << *(navData->G_ptr) << " ";
        std::cout << *(navData->G_ptr+1) << " ";
        std::cout << *(navData->G_ptr+2) << " ";
        std::cout << std::endl;

        std::cout << "M ";
        std::cout << *(navData->M_ptr) << " ";
        std::cout << *(navData->M_ptr+1) << " ";
        std::cout << *(navData->M_ptr+2) << " ";
        std::cout << (navData->IMU_ptr->magModule) << " ";
        std::cout << std::endl;

        std::cout << "----------------------------------"<<" \n";

        vTaskDelay(SYSTEM_SAMPLE_PERIOD_MS/portTICK_PERIOD_MS); //TODO: Setar taxa fixa para execução da Navegação
    }
}
