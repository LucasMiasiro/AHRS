#pragma once
#include "config.h"
#include "esp_err.h"

class GY87 {
private:
    esp_err_t write(uint8_t* data_ptr, const size_t size, const uint8_t address, const uint8_t register_address);
    esp_err_t read(uint8_t* buffer_ptr, const size_t size, const uint8_t address, const uint8_t register_address);
    bool setup_i2c();
    bool calibrate_mag(float*);
    const float magCal[6] = {GY87_MAG_CAL};

    int16_t A_raw[3] = {0, 0, 0};
    int16_t G_raw[3] = {0, 0, 0};
    int16_t M_raw[3] = {0, 0, 0};
    int32_t A_raw_accum[3] = {0, 0, 0};
    int32_t G_raw_accum[3] = {0, 0, 0};
    int32_t M_raw_accum[3] = {0, 0, 0};
    uint8_t N_samples = 0;
public:

    GY87();
    esp_err_t setup();
    esp_err_t read_test();
    bool calibrate_loop();

    bool read_data();
    bool get_data(float*, float*, float*);
    bool accumulate_data();
    bool clean_accum();
    uint8_t get_N_samples();
};
