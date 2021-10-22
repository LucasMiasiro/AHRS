#pragma once
#include "config.h"

class DCM {
private:
    float q[4] = {0};
    float q_dot[4] = {0};
    float q_G[4] = {0};
    float q_dot_G[4] = {0};
    float q_AM[4] = {0};
    float JT[4][6]; 
    float f[6]; 
    float grad_f[4];
    float mu_t = 0;
    float gamma_t = 0;

    float buf_q[4] = {0};
    float buf2_q[4] = {0};
    float G_q[4] = {0};
    float M_q[4] = {0};
    float earthMagField[3] = {WMM_LOCALFIELD};

    void initializeAccelTransposedJacobian(float *, float *, float *);
    void initializeMagTransposedJacobian(float *, float *, float *);
    void magAccelPred(float*, float*);
    void gyroPred(float*);
    void initializeVariables(float*, float*, float*);
    void filterFusion();
    void softIronCompensation(float *);
    void gyroBiasDriftCompensation(float *);

public:
    DCM();
    void update(float*, float*, float*);
    void getStates(float*, float*);
};
