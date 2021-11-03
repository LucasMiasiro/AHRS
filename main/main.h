#pragma once
#include "NEO-M8N.h"
//#include "ATGM336.h"
#include "GY87.h"

struct navData_ptr{
    GY87* IMU_ptr;
    NEOM8N* GNSS_ptr;
    // ATGM336* GNSS_ptr;
    float* A_ptr;
    float* G_ptr;
    float* M_ptr;
    float* eulerAngles_ptr;
    float* eulerAngRates_ptr;
    float* pos_ptr;
    float* vel_ptr;
};
void sensorTask(void*);
void navTask(void*);
void sendTask(void*);
void gyroCalTask(void*);
void magCalTask(void*);
