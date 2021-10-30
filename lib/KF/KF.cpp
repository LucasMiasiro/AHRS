#include "KF.h"
#include "config.h"

#include "vMath.h"
#include <math.h> 
#include "serial-logger.h"

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

    if (states[0] > 1.0){ //TODO: remover
        reset();
    } else if (states[0] < -1.0) {
        reset();
    }
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

    /* serialLogger::logFloat(states, 6, "KF"); */
}

KF::KF(){
};
