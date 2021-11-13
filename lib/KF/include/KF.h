#pragma once
#include "config.h"
#include "math.h"

#include "DCM.h"
#include "main.h"
#include "NEO-M8N.h"

class KF {
private:
    float states[6] = {0};
    float buf_6[6] = {0};
    float meas[6] = {0};

    const float Qx[2] = {KF_POSXY_Q};
    const float Qy[2] = {KF_POSXY_Q};
    const float Qz[2] = {0.1, 0.1};
    const float Rx[2] = {KF_POSXY_R};
    const float Ry[2] = {KF_POSXY_R};
    const float Rz[2] = {0.1, 0.1};

    float Px[2][2] = {{0.1, 0.0},
                      {0.0, 1.0}};
    float Py[2][2] = {{0.1, 0.0},
                      {0.0, 1.0}};
    float Pz[2][2] = {{0.1, 0.0},
                      {0.0, 1.0}};

    float Kx[2][2] = {{0}};
    float Ky[2][2] = {{0}};
    float Kz[2][2] = {{0}};

    float buf_2x2[2][2] = {{0}};

    const float dt = SYSTEM_SAMPLE_PERIOD_MS/1000.0f;
    const float hdtsG = GRAVITY*dt*dt/2;
    void propagateCovarianceMatrixPriori(float P[2][2], const float Q[2]);
    void propagateCovarianceMatrixPosteriori(float P[2][2], float K[2][2]);
    void propagateStates(float *);
    void calcKalmanGain(float K[2][2], float P[2][2], const float R[2]);
    void predict(float*);
    void correct();
    void fuseGNSS();
    void fuseBaro();
    void reset();

    DCM *_DCM;
    navData_ptr *_navData;

public:
    KF();
    KF(DCM *DCM, navData_ptr *&navData);
    void getStates(float*, float*);
    void update(float*);
};
