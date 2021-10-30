#pragma once
#include "config.h"
#include "math.h"

class KF {
private:
    float states[6] = {0};
    const float dt = SYSTEM_SAMPLE_PERIOD_MS/1000.0f;
    const float half_dt_squared = dt*dt/2;
    void predict(float*);
    void reset();

public:
    KF();
    void getStates(float*, float*);
    void update(float*);
};
