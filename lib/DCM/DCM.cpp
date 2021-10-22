#include "DCM.h"
#include "config.h"
#include "iostream" //TODO:remover

#include "vMath.h"
#include <math.h> 

void DCM::getStates(float *eulerAngles, float *eulerRates){
    eulerAngles[0] = atan2(2.0*(q[2]*q[3] - q[0]*q[1]),
                            2.0*(q[0]*q[0] + q[3]*q[3]) - 1); //Phi

    eulerAngles[1] = -asin(2*(q[1]*q[3] + q[0]*q[2])); //Theta

    eulerAngles[0] = atan2(2.0*(q[1]*q[2] - q[0]*q[3]),
                            2.0*(q[0]*q[0] + q[1]*q[1]) - 1); //Psi

    //TODO: Implementar eulerRates
}

void DCM::update(float* A, float* G, float* M){

    //TODO: normalizar acelerômetro?
    initializeVariables(A, G, M);

    gyroPred(G);
    magAccelPred(A, M);

    filterFusion();
}

void DCM::filterFusion(){
    gamma_t = BETA/(mu_t*1000/SYSTEM_SAMPLE_PERIOD_MS + BETA);
    quatProdConst(q_AM, gamma_t, buf_q);
    quatProdConst(q_G, 1-gamma_t, buf2_q);

    quatAdd(buf_q, buf2_q, q);
}

void DCM::magAccelPred(float* A, float* M){
    matProd_4x6x1(&JT[0][0], f, buf_q);
    mu_t = ALPHA*norm2(q_dot_G, 4)*SYSTEM_SAMPLE_PERIOD_MS/1000;
    quatProdConst(q_AM, -mu_t/norm2(buf_q, 4), buf2_q);
    quatAdd(q, buf2_q, q_AM);
}

void DCM::initializeVariables(float* A, float *G, float *M){
    G_q[1] = G[0];
    G_q[2] = G[1];
    G_q[3] = G[2];

#if COMPENSATE
    softIronCompensation(M);
#endif
    initializeAccelTransposedJacobian(A, &JT[0][0], f);
    initializeMagTransposedJacobian(M, &JT[3][0], f+3); //TODO: checar se endereço está correto e fazendo sentido
}

void DCM::softIronCompensation(float *M){
    M_q[0] = 0;
    M_q[1] = M[0];
    M_q[2] = M[1];
    M_q[3] = M[2];

    quatConj(q, buf_q);
    quatProd(M_q, buf_q, buf2_q);
    quatProd(q, buf2_q, buf_q);

    earthMagField[1] = sqrt(buf_q[1]*buf_q[1] + buf_q[2]+buf_q[2]);
    earthMagField[2] = buf_q[3];
}

void DCM::gyroBiasDriftCompensation(float *G){
}

void DCM::initializeMagTransposedJacobian(float* M, float *Matrix, float* f){
    *((Matrix)) = -2.0*q[2]*earthMagField[2];
    *((Matrix+1)) = 2.0*q[3]*earthMagField[2];
    *((Matrix+2)) = -2.0*q[0]*earthMagField[2] - 4.0*q[2]*earthMagField[0];
    *((Matrix+3)) = 2.0*q[1]*earthMagField[2] - 4.0*q[3]*earthMagField[0];

    *((Matrix)+1) = 2.0*q[1]*earthMagField[2] - 2.0*q[3]*earthMagField[0];
    *((Matrix+1)+1) = 2.0*q[0]*earthMagField[2] + 2.0*q[2]*earthMagField[0];
    *((Matrix+2)+1) = 2.0*q[3]*earthMagField[2] + 2.0*q[1]*earthMagField[0];
    *((Matrix+3)+1) = 2.0*q[2]*earthMagField[2] - 2.0*q[0]*earthMagField[0];

    *((Matrix)+2) = 2.0*q[2]*earthMagField[0];
    *((Matrix+1)+2) = -4.0*q[1]*earthMagField[2] + 2.0*q[3]*earthMagField[0];
    *((Matrix+2)+2) = -4.0*q[1]*earthMagField[2] + 2.0*q[0]*earthMagField[0];
    *((Matrix+3)+2) = 2.0*q[1]*earthMagField[0];

    f[0] =  2.0*(0.5f - q[2]*q[2] - q[3]*q[3])*earthMagField[0]
            + 2.0*(q[1]*q[3] - q[0]*q[2])*earthMagField[2] - M[0];
    f[1] =  2.0*(q[1]*q[2] - q[0]*q[3])*earthMagField[0]
            + 2.0*(q[0]*q[1] + q[2]*q[3])*earthMagField[2] - M[1];
    f[2] =  2.0*(q[0]*q[2] + q[1]*q[3])*earthMagField[0]
            + 2.0*(0.5 - q[1]*q[1] - q[2]*q[2])*earthMagField[2] - M[2];
}

void DCM::initializeAccelTransposedJacobian(float* A, float *Matrix, float *f){
    *((Matrix)) = -2.0*q[2];
    *((Matrix+1)) = 2.0*q[3];
    *((Matrix+2)) = -2.0*q[0];
    *((Matrix+3)) = 2.0*q[1];

    *((Matrix)+1) = 2.0*q[1];
    *((Matrix+1)+1) = 2.0*q[0];
    *((Matrix+2)+1) = 2.0*q[3];
    *((Matrix+3)+1) = 2.0*q[2];

    *((Matrix)+2) = 0;
    *((Matrix+1)+2) = -4.0*q[1];
    *((Matrix+2)+2) = -4.0*q[1];
    *((Matrix+3)+2) = 0;

    f[0] = 2.0*(q[1]*q[3] - q[0]*q[2]) - A[0];
    f[1] = 2.0*(q[0]*q[1] + q[2]*q[3]) - A[1];
    f[2] = 2.0*(0.5 - q[1]*q[1] - q[2]*q[2]) - A[2];
}


void DCM::gyroPred(float* G){

    quatProd(G_q, q, buf_q);
    quatProdConst(buf_q, 0.5, q_dot_G); // q_dot from gyro

    quatProdConst(q_dot_G, SYSTEM_SAMPLE_PERIOD_MS/1000, buf_q);
    quatAdd(q, buf_q, q_G); // q from gyro
}


DCM::DCM(){
};
