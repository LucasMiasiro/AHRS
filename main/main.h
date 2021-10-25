#pragma once
#include "GY87.h"

#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"

struct navData_ptr{
    GY87* IMU_ptr;
    float* A_ptr;
    float* G_ptr;
    float* M_ptr;
    float* eulerAngles_ptr;
    float* eulerAngRates_ptr;
};
void sensorTask(void*);
void navTask(void*);
void sendTask(void*);
