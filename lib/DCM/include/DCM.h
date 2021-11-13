#pragma once
#include "config.h"
#include "math.h"

class DCM {
private:
    float q[4] = {0};
    float q_dot[4] = {0};
    float q_dot_G[4] = {0};
    float JT[4][6] = {{0}}; 
    float f[6]; 
    float grad_f_normalized[4];

    const float beta = sqrt(3.0/4.0f)*GYRO_ERROR*DEG2RAD;
    /* const float zeta = sqrt(3.0/4.0f)*GYRO_DRIFT*DEG2RAD; */
    float buf_q[4] = {0.0f};
    float buf2_q[4] = {0.0f};
    float G_q[4] = {0.0f};
    float M_q[4] = {0.0f};
    float A_q[4] = {0.0f};
    float earthMagField[3] = {WMM_LOCALFIELD};

    void initializeAccelTransposedJacobian(float[4][6], float *);
    void initializeMagTransposedJacobian(float[4][6], float *);
    void magAccelPred();
    void gyroPred();
    void initializeVariables(float*, float*, float*);
    void filterFusion();
    void softIronCompensation();
    void gyroBiasDriftCompensation(float *);

public:
    DCM();
    void update(float*, float*, float*);
    void getStates(float*, float*);
    void initializeFilter();
    void rotate2Earth(float *v);
    void rotate2Earth(float M[3][3], float v[3]);
    void getDCM(float M[3][3], float *q);
};
