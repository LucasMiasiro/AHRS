#include "DCM.h"
#include "config.h"
#include "iostream" //TODO:remover

#include "vMath.h"
#include <math.h> 

void DCM::getStates(float *eulerAngles, float *eulerRates){
    eulerAngles[0] = atan2(2.0*(q[2]*q[3] - q[0]*q[1]),
                            2.0*(q[0]*q[0] + q[3]*q[3]) - 1); //Phi

    eulerAngles[1] = -asin(2*(q[1]*q[3] + q[0]*q[2])); //Theta

    eulerAngles[2] = atan2(2.0*(q[1]*q[2] - q[0]*q[3]),
                            2.0*(q[0]*q[0] + q[1]*q[1]) - 1); //Psi

    //TODO: Implementar eulerRates
}

void DCM::update(float* A, float* G, float* M){

    initializeVariables(A, G, M);

    gyroPred(G);
    magAccelPred(A, M);

    std::cout << "Q ";
    std::cout << q_G[0] << " ";
    std::cout << q_G[1] << " ";
    std::cout << q_G[2] << " ";
    std::cout << q_G[2] << " ";
    std::cout << q_dot_G[0] << " ";
    std::cout << q_dot_G[1] << " ";
    std::cout << q_dot_G[2] << " ";
    std::cout << q_dot_G[3] << " ";
    std::cout << q_AM[0] << " ";
    std::cout << q_AM[1] << " ";
    std::cout << q_AM[2] << " ";
    std::cout << q_AM[3] << " ";
    std::cout << std::endl;

    filterFusion();
}

void DCM::initializeFilter(){
    float r[3] = {0, 0, 1};
    float psi = 0;
    q[0] = cos(psi/2);
    q[1] = -r[0]*sin(psi/2);
    q[2] = -r[1]*sin(psi/2);
    q[3] = -r[2]*sin(psi/2);
    normalizeQuat(q);
}

void DCM::filterFusion(){
    gamma_t = BETA/(mu_t*1000/SYSTEM_SAMPLE_PERIOD_MS + BETA);
    quatProdConst(q_AM, gamma_t, buf_q);
    quatProdConst(q_G, 1-gamma_t, buf2_q);
    quatAdd(buf_q, buf2_q, q);
    normalizeQuat(q);
}

void DCM::magAccelPred(float* A, float* M){

    /* std::cout << "MATRIX\n"; */
    /* for (int i = 0; i < 4; i++){ */
    /*     for (int j = 0; j < 6; j++){ */
    /*         std::cout << i << ", " << j << " -> " << JT[i][j] << "\n"; */
    /*     } */
    /* } */

    matProd_4x6x1(&JT[0][0], f, buf_q);
    mu_t = norm2(q_dot_G, 4)*ALPHA*SYSTEM_SAMPLE_PERIOD_MS/1000;
    normalizeQuat(buf_q);
    quatProdConst(buf_q, -mu_t, buf2_q);

    std::cout << "BUF ";
    std::cout << mu_t << " ";
    std::cout << buf_q[0] << " ";
    std::cout << buf_q[1] << " ";
    std::cout << buf_q[2] << " ";
    std::cout << buf_q[3] << " ";
    /* std::cout << buf2_q[0] << " "; */
    /* std::cout << buf2_q[1] << " "; */
    /* std::cout << buf2_q[2] << " "; */
    /* std::cout << buf2_q[3] << " "; */
    std::cout << std::endl;

    quatAdd(q, buf2_q, q_AM);
}

void DCM::initializeVariables(float* A, float *G, float *M){
    G_q[1] = G[0];
    G_q[2] = G[1];
    G_q[3] = G[2];

#if COMPENSATE
    softIronCompensation(M);
    gyroBiasDriftCompensation(G);
#endif
    initializeAccelTransposedJacobian(A, JT, f);
    initializeMagTransposedJacobian(M, JT, f); //TODO: checar se endereço está correto e fazendo sentido
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

void DCM::initializeMagTransposedJacobian(float* M, float Matrix[4][6], float* f){
    Matrix[0][3] = -2.0*q[2]*earthMagField[2];
    Matrix[1][3] = 2.0*q[3]*earthMagField[2];
    Matrix[2][3] = -2.0*q[0]*earthMagField[2] - 4.0*q[2]*earthMagField[0];
    Matrix[3][3] = 2.0*q[1]*earthMagField[2] - 4.0*q[3]*earthMagField[0];

    Matrix[0][4] = 2.0*q[1]*earthMagField[2] - 2.0*q[3]*earthMagField[0];
    Matrix[1][4] = 2.0*q[0]*earthMagField[2] + 2.0*q[2]*earthMagField[0];
    Matrix[2][4] = 2.0*q[3]*earthMagField[2] + 2.0*q[1]*earthMagField[0];
    Matrix[3][4] = 2.0*q[2]*earthMagField[2] - 2.0*q[0]*earthMagField[0];

    Matrix[0][5] = 2.0*q[2]*earthMagField[0];
    Matrix[1][5] = -4.0*q[1]*earthMagField[2] + 2.0*q[3]*earthMagField[0];
    Matrix[2][5] = -4.0*q[1]*earthMagField[2] + 2.0*q[0]*earthMagField[0];
    Matrix[3][5] = 2.0*q[1]*earthMagField[0];

    f[3] =  2.0*(0.5f - q[2]*q[2] - q[3]*q[3])*earthMagField[0]
            + 2.0*(q[1]*q[3] - q[0]*q[2])*earthMagField[2] - M[0];

    f[4] =  2.0*(q[1]*q[2] - q[0]*q[3])*earthMagField[0]
            + 2.0*(q[0]*q[1] + q[2]*q[3])*earthMagField[2] - M[1];

    f[5] =  2.0*(q[0]*q[2] + q[1]*q[3])*earthMagField[0]
            + 2.0*(0.5 - q[1]*q[1] - q[2]*q[2])*earthMagField[2] - M[2];
}

void DCM::initializeAccelTransposedJacobian(float* A, float Matrix[4][6], float *f){
    Matrix[0][0] = -2.0*q[2];
    Matrix[1][0] = 2.0*q[3];
    Matrix[2][0] = -2.0*q[0];
    Matrix[3][0] = 2.0*q[1];

    Matrix[0][1] = 2.0*q[1];
    Matrix[1][1] = 2.0*q[0];
    Matrix[2][1] = 2.0*q[3];
    Matrix[3][1] = 2.0*q[2];

    Matrix[0][2] = 0;
    Matrix[1][2] = -4.0*q[1];
    Matrix[2][2] = -4.0*q[1];
    Matrix[3][2] = 0;

    f[0] = 2.0*(q[1]*q[3] - q[0]*q[2]) - A[0];
    f[1] = 2.0*(q[0]*q[1] + q[2]*q[3]) - A[1];
    f[2] = 2.0*(0.5 - q[1]*q[1] - q[2]*q[2]) - A[2];
}


void DCM::gyroPred(float* G){

    quatProd(q, G_q, buf_q);
    quatProdConst(buf_q, 0.5, q_dot_G); // q_dot from gyro

    quatProdConst(q_dot_G, SYSTEM_SAMPLE_PERIOD_MS/1000, buf_q);
    quatAdd(q, buf_q, q_G); // q from gyro
}


DCM::DCM(){
};
