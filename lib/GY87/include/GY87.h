#pragma once
#include "config.h"
#include "esp_err.h"

enum BAROSTATE {
    SHOULD_READ_TEMP,
    WAIT_FOR_TEMP,
    READ_TEMP,
    SHOULD_READ_PRESSURE,
    WAIT_FOR_PRESSURE,
    READ_PRESSURE
};

class GY87 {
private:
    esp_err_t write(uint8_t*, const size_t, const uint8_t, const uint8_t);
    esp_err_t read(uint8_t*, const size_t, const uint8_t, const uint8_t);
    bool setup_i2c();
    bool calibrateMag(float*);
    bool readData();
    bool cleanAccum();
    bool setupBaro();
    void setPressureMeas();
    void setTemperatureMeas();
    void getBaroCal();
    void calcTruePressure();
    void calcTruePressureAccum();
    float calcAlt();

    uint8_t buffer[22];
    float fbuffer;
    uint8_t opt;

    int16_t AC1, AC2, AC3, B1, B2, MB, MC, MD;
    uint16_t AC4, AC5, AC6;
    int32_t X1, X2, X3, B3, B5, B6;
    uint32_t B4, B7;
    int32_t T = 250, p = 0;
    float lastB = 500;
    BAROSTATE baroState = SHOULD_READ_TEMP;

    const float magCal[6] = {GY87_MAG_CAL};
    const float gyroCal[6] = {GY87_GYRO_CAL};
    int16_t A_raw[3] = {0, 0, 0};
    int16_t G_raw[3] = {0, 0, 0};
    int16_t M_raw[3] = {0, 0, 0};
    int32_t B_raw = 0;
    int32_t T_raw = 0;
    int32_t A_raw_accum[3] = {0, 0, 0};
    int32_t G_raw_accum[3] = {0, 0, 0};
    int32_t M_raw_accum[3] = {0, 0, 0};
    uint32_t B_raw_accum = 0;
    uint16_t N_samples = 0;

public:

    GY87();
    bool getData(float*, float*, float*, float*);
    bool accumulateData();
    bool magCalibrationLoop();
    bool gyroCalibrationLoop();
    void setHome();
    void getPosZ(float *);

    float Home_Alt;
    float magModule = 0;
};
