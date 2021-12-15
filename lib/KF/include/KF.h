#pragma once
#include "config.h"
#include "math.h"
#include "vMath.h"

#include "DCM.h"
#include "main.h"
#include "NEO-M8N.h"

class KF {
private:
    const float dt = SYSTEM_SAMPLE_PERIOD_MS/1000.0f;
    const float dtG = GRAVITY*dt;
    const float hdtsG = GRAVITY*dt*dt/2;
    const float stdDevRatio = GRAVITY*ACCEL_STD_DEV/BARO_STD_DEV;

    float states[6] = {0};
    float buf_6[6] = {0};
    float meas[6] = {0};

    const float Qx[2] = {sq(hdtsG*ACCEL_STD_DEV), sq(dtG*ACCEL_STD_DEV)};
    const float Qy[2] = {sq(hdtsG*ACCEL_STD_DEV), sq(dtG*ACCEL_STD_DEV)};
    const float Rx[2] = {sq(GNSS_POS_STD_DEV), sq(GNSS_VEL_STD_DEV)};
    const float Ry[2] = {sq(GNSS_POS_STD_DEV), sq(GNSS_VEL_STD_DEV)};

    float Px[2][2] = {{0.1, 0.0},
                      {0.0, 0.01}};
    float Py[2][2] = {{0.1, 0.0},
                      {0.0, 0.01}};

    float Kx[2][2] = {{0}};
    float Ky[2][2] = {{0}};

    float Kz[2] = {sqrt(2*stdDevRatio)*dt + stdDevRatio*dt*dt/2,
                    stdDevRatio*dt};

    float buf_2x2[2][2] = {{0}};
    float z_prev = 0, x_gnss_prev = 0, y_gnss_prev = 0, Uder = 0;


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
