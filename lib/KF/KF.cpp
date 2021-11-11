#include "KF.h"
#include "config.h"

#include "vMath.h"
#include <math.h> 
#include "serial-logger.h"

#include "DCM.h"
#include "main.h"
#include "NEO-M8N.h"

void KF::getStates(float *pos, float *vel){
    pos[0] = states[0];
    pos[1] = states[1];
    pos[2] = states[2];

    vel[0] = states[3];
    vel[1] = states[4];
    vel[2] = states[5];
}

void KF::update(float *A_E){
    predict(A_E);
    correct();
}

void KF::correct(){
    if (_navData->GNSS_ptr->newData){
        fuseGNSS();
        _navData->GNSS_ptr->newData = 0;
    }

    states[4] = _navData->IMU_ptr->Home_Alt - *(_navData->B_ptr);
    states[5] = 0;
}

void KF::fuseGNSS(){

    // A posteriori estimate 
    calcKalmanGain(Kx, Px, Rx);
    calcKalmanGain(Ky, Py, Rz);

    _navData->GNSS_ptr->getPos(&meas[0], &meas[1]);

    meas[2] = cosf(_navData->GNSS_ptr->GNSS.cog * DEG2RAD)
                    * _navData->GNSS_ptr->GNSS.speed;
    meas[3] = sinf(_navData->GNSS_ptr->GNSS.cog * DEG2RAD)
                    * _navData->GNSS_ptr->GNSS.speed;

    buf_6[0] = meas[0] - states[0];
    buf_6[1] = meas[1] - states[1];
    matProd_2x2x1(&Kx[0][0], &buf_6[0], &buf_6[0]);
    states[0] += buf_6[0];
    states[1] += buf_6[1];

    buf_6[2] = meas[2] - states[2];
    buf_6[3] = meas[3] - states[3];
    matProd_2x2x1(&Ky[0][0], &buf_6[2], &buf_6[2]);
    states[2] += buf_6[2];
    states[3] += buf_6[3];
    
    //A posteriori covariance matrix propagation
    propagateCovarianceMatrixPosteriori(Px, Kx);
    propagateCovarianceMatrixPosteriori(Py, Ky);
}

void KF::propagateCovarianceMatrixPosteriori(float P[2][2], float K[2][2]){
    buf_2x2[0][0] = 1 - K[0][0];
    buf_2x2[1][0] = -K[1][0];
    buf_2x2[0][1] = -K[0][1];
    buf_2x2[1][1] = 1 - K[1][1];

    matProd_2x2x2(&buf_2x2[0][0], &P[0][0], &P[0][0]);
}

void KF::calcKalmanGain(float K[2][2], float P[2][2], const float R[2]){
    buf_2x2[0][0] = P[0][0] + R[0];
    buf_2x2[1][0] = P[1][0];
    buf_2x2[0][1] = P[0][1];
    buf_2x2[1][1] = P[1][1] + R[1];

    inv2(buf_2x2);
    matProd_2x2x2(&P[0][0], &buf_2x2[0][0], &K[0][0]);
}

void KF::reset(){
    states[0] = 0;
    states[1] = 0;
    states[2] = 0;
    states[3] = 0;
    states[4] = 0;
    states[5] = 0;
}

void KF::predict(float *A_E){
    A_E[3] += 1;

    //A priori estimate
    propagateStates(A_E);

    //A priori covariance matrix propagation
    propagateCovarianceMatrixPriori(Px, Qx);
    propagateCovarianceMatrixPriori(Py, Qy);
    propagateCovarianceMatrixPriori(Pz, Qz);
}

void KF::propagateStates(float *A_E){
    buf_6[3] = states[3];
    buf_6[4] = states[4];
    buf_6[5] = states[5];

    states[0] += A_E[1]*hdtsG + buf_6[3]*dt;
    states[1] += A_E[2]*hdtsG + buf_6[4]*dt;
    states[2] += A_E[3]*hdtsG + buf_6[5]*dt;
    states[3] += A_E[1]*dt;
    states[4] += A_E[2]*dt;
    states[5] += A_E[3]*dt;
}

void KF::propagateCovarianceMatrixPriori(float P[2][2], const float Q[2]){
    buf_2x2[0][0] = P[0][0];
    buf_2x2[1][0] = P[1][0];
    buf_2x2[0][1] = P[0][1];
    buf_2x2[1][1] = P[1][1];

    P[0][1] = dt*buf_2x2[1][1] + buf_2x2[0][1];
    P[0][0] = Q[0] + dt*P[0][1] + dt*buf_2x2[1][0] + buf_2x2[0][0];
    P[1][0] = dt*buf_2x2[1][1] + buf_2x2[1][0];
    P[1][1] = Q[1] + buf_2x2[1][1];
}



KF::KF(DCM *DCM, navData_ptr *&navData){
    _DCM = DCM;
    _navData = navData;
};

KF::KF(){
};
