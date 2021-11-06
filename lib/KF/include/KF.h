#pragma once
#include "config.h"
#include "math.h"

#include "DCM.h"
#include "main.h"
#include "NEO-M8N.h"

class KF {
private:
    float states[6] = {0};
    const float dt = SYSTEM_SAMPLE_PERIOD_MS/1000.0f;
    const float half_dt_squared = dt*dt/2;
    void predict(float*);
    void correct();
    void reset();

    DCM *_DCM;
    navData_ptr *_navData;

public:
    KF();
    KF(DCM *DCM, navData_ptr *&navData);
    void getStates(float*, float*);
    void update(float*);
};
