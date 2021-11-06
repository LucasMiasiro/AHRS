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
    if (states[0] > 30.0 || states[1] > 30.0){ //TODO: remover
        reset();
    } else if (states[0] < -30.0  || states[1] < -30.0) {
        reset();
    }

    if (_navData->GNSS_ptr->newData){ //TODO implementar
        _navData->GNSS_ptr->getPos(&states[0], &states[1]);
        if (_navData->GNSS_ptr->GNSS.speed > GNSS_FUSION_MIN_VEL) {
            states[3] = cosf(_navData->GNSS_ptr->GNSS.cog * DEG2RAD)
                            * _navData->GNSS_ptr->GNSS.speed;
            states[4] = sinf(_navData->GNSS_ptr->GNSS.cog * DEG2RAD)
                            * _navData->GNSS_ptr->GNSS.speed;
        } else {
            states[3] = 0;
            states[4] = 0;
        }
        _navData->GNSS_ptr->newData = 0;
    }
    states[2] = _navData->IMU_ptr->Home_Alt - *(_navData->B_ptr);
    states[5] = 0;

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
    A_E[3] += GRAVITY;

    states[3] += A_E[1]*dt;
    states[4] += A_E[2]*dt;
    states[5] += A_E[3]*dt;

    states[0] += A_E[1]*half_dt_squared + states[3]*dt;
    states[1] += A_E[2]*half_dt_squared + states[4]*dt;
    states[2] += A_E[3]*half_dt_squared + states[5]*dt;
}


KF::KF(DCM *DCM, navData_ptr *&navData){
    _DCM = DCM;
    _navData = navData;
};

KF::KF(){

};
