#pragma once
#include "config.h"
#include "esp_err.h"

class GY87 {
private:
    esp_err_t write(uint8_t* data_ptr, const size_t size, const uint8_t address, const uint8_t register_address);
    esp_err_t read(uint8_t* buffer_ptr, const size_t size, const uint8_t address, const uint8_t register_address);
    bool setup_i2c();
    float magCal[6] = {GY87_MAG_CAL};
public:
    int16_t A_raw[3] = {0, 0, 0};
    int32_t A_raw_accum[3] = {0, 0, 0};
    uint8_t N_samples = 0;
    esp_err_t setup();
    esp_err_t read_test();
    bool calibrate_mag(float *mag_min_max);
};
