#pragma once
#include "config.h"
#include "esp_err.h"

class GY87 {
private:
    esp_err_t write(uint8_t*, const size_t, const uint8_t, const uint8_t);
    esp_err_t read(uint8_t*, const size_t, const uint8_t, const uint8_t);
    bool setup_i2c();
    bool calibrateMag(float*);
    bool readData();
    bool cleanAccum();

    const float magCal[6] = {GY87_MAG_CAL};
    const float gyroCal[6] = {GY87_GYRO_CAL};
    int16_t A_raw[3] = {0, 0, 0};
    int16_t G_raw[3] = {0, 0, 0};
    int16_t M_raw[3] = {0, 0, 0};
    int32_t A_raw_accum[3] = {0, 0, 0};
    int32_t G_raw_accum[3] = {0, 0, 0};
    int32_t M_raw_accum[3] = {0, 0, 0};
    uint8_t N_samples = 0;

public:

    GY87();
    bool getData(float*, float*, float*);
    bool accumulateData();
    bool magCalibrationLoop();
    bool gyroCalibrationLoop();

    float magModule = 0;
};
