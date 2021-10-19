#pragma once
#include "GY87.h"

struct navData_ptr{
    GY87* IMU_ptr;
    float* A_ptr;
    float* G_ptr;
    float* M_ptr;
};
void sensorTask(void*);
void navTask(void*);
void navTask2(void*);