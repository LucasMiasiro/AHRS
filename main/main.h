#pragma once
#include "ATGM336.h"
#include "GY87.h"

struct navData_ptr{
    GY87* IMU_ptr;
    ATGM336* GNSS_ptr;
    float* A_ptr;
    float* G_ptr;
    float* M_ptr;
    float* eulerAngles_ptr;
    float* eulerAngRates_ptr;
};
void sensorTask(void*);
void navTask(void*);
void sendTask(void*);
void gyroCalTask(void*);
void magCalTask(void*);
