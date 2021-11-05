#include "GY87.h"
#include "driver/i2c.h"
#include "config.h"
#include "gpio-handler.h"
#include <math.h> 
#include "serial-logger.h"

bool GY87::accumulateData(){
    if (readData()){
        return 1;
    }

    A_raw_accum[0] += A_raw[0];
    A_raw_accum[1] += A_raw[1];
    A_raw_accum[2] += A_raw[2];

    G_raw_accum[0] += G_raw[0];
    G_raw_accum[1] += G_raw[1];
    G_raw_accum[2] += G_raw[2];

    M_raw_accum[0] += M_raw[0];
    M_raw_accum[1] += M_raw[1];
    M_raw_accum[2] += M_raw[2];

    B_raw_accum += B_raw;

    N_samples++;

    return 0;
}

void GY87::setHome(){
    float A_temp[3], G_temp[3], M_temp[3], B_temp = 0;
    float B_accum = 0;
    uint8_t N_accum = 0;

    for (int j = 0; j < 20; j++){
        accumulateData();
    }
    getData(A_temp, G_temp, M_temp, &B_temp);

    for (int i = 0; i < 10; i++){
        for (int j = 0; j < 20; j++){
            accumulateData();
        }
        getData(A_temp, G_temp, M_temp, &B_temp);
        B_accum += B_temp;
        N_accum++;
    }

    Home_Alt = B_accum/N_accum;
}

bool GY87::getData(float* A, float* G, float* M, float* B){

    baroState = SHOULD_READ_TEMP;

    if (N_samples == 0){
        *A = A_raw[0]*GY87_ACCEL_SENS;
        *(A+1) = A_raw[1]*GY87_ACCEL_SENS;
        *(A+2) = A_raw[2]*GY87_ACCEL_SENS;

        *G = G_raw[0]*GY87_GYRO_SENS - gyroCal[0];
        *(G+1) = G_raw[1]*GY87_GYRO_SENS - gyroCal[1];
        *(G+2) = G_raw[2]*GY87_GYRO_SENS - gyroCal[2];

        *M = (M_raw[0]*GY87_MAG_SENS - magCal[0])*magCal[3];
        *(M+1) = (M_raw[1]*GY87_MAG_SENS - magCal[1])*magCal[4];
        *(M+2) = (M_raw[2]*GY87_MAG_SENS - magCal[2])*magCal[5];

        calcTruePressure();
        *B = calcAlt();

        return 1;
    }

    *A = A_raw_accum[0]*GY87_ACCEL_SENS/N_samples;
    *(A+1) = A_raw_accum[1]*GY87_ACCEL_SENS/N_samples;
    *(A+2) = A_raw_accum[2]*GY87_ACCEL_SENS/N_samples;

    *G = G_raw_accum[0]*GY87_GYRO_SENS/N_samples - gyroCal[0];
    *(G+1) = G_raw_accum[1]*GY87_GYRO_SENS/N_samples - gyroCal[1];
    *(G+2) = G_raw_accum[2]*GY87_GYRO_SENS/N_samples - gyroCal[2];

    *M = (M_raw_accum[0]*GY87_MAG_SENS/N_samples - magCal[0])*magCal[3];
    *(M+1) = (M_raw_accum[1]*GY87_MAG_SENS/N_samples - magCal[1])*magCal[4];
    *(M+2) = (M_raw_accum[2]*GY87_MAG_SENS/N_samples - magCal[2])*magCal[5];

    magModule = sqrt((*M)*(*M) + (*(M+1))*(*(M+1)) + (*(M+2))*(*(M+2)));

    calcTruePressureAccum();
    *B = calcAlt();

    cleanAccum();

    return 0;
}

bool GY87::calibrateMag(float *mag_min_max){
    float A_temp[3], G_temp[3], M_temp[3], B_temp;
    float magCal_temp[6] = {0};
    float rx, ry, rz, r, mag1, mag2, mag3;
    bool newMax = 0;

    for (int i = 0; i < 100; i++){
        accumulateData();
    }
    
    getData(A_temp, G_temp, M_temp, &B_temp);

    mag1 = M_temp[0];
    mag2 = M_temp[1];
    mag3 = M_temp[2];

    if (*mag_min_max == 0){
        *mag_min_max = mag1;
        *(mag_min_max+1) = mag1;
        *(mag_min_max+2) = mag2;
        *(mag_min_max+3) = mag2;
        *(mag_min_max+4) = mag3;
        *(mag_min_max+5) = mag3;
        return 0;
    }
    
    if (mag1 < *mag_min_max){
        *mag_min_max = mag1;
        newMax = 1;
        }
    if (mag1 > *(mag_min_max+1)){
        *(mag_min_max+1) = mag1;
        newMax = 1;
        }

    if (mag2 < *(mag_min_max+2)){
        *(mag_min_max+2) = mag2;
        newMax = 1;
        }
    if (mag2 > *(mag_min_max+3)){
        *(mag_min_max+3) = mag2;
        newMax = 1;
        }

    if (mag3 < *(mag_min_max+4)){
        *(mag_min_max+4) = mag3;
        newMax = 1;
        }
    if (mag3 > *(mag_min_max+5)){
        *(mag_min_max+5) = mag3;
        newMax = 1;
        }

    if (newMax){
        magCal_temp[0] = (*(mag_min_max) + *(mag_min_max+1))/2;
        magCal_temp[1] = (*(mag_min_max+2) + *(mag_min_max+3))/2;
        magCal_temp[2] = (*(mag_min_max+4) + *(mag_min_max+5))/2;
        rx = (*(mag_min_max+1) - *(mag_min_max))/2;
        ry = (*(mag_min_max+3) - *(mag_min_max+2))/2;
        rz = (*(mag_min_max+5) - *(mag_min_max+4))/2;
        r = cbrt(rx*ry*rz);
        magCal_temp[3] = r/rx;
        magCal_temp[4] = r/ry;
        magCal_temp[5] = r/rz;

        serialLogger::logFloat(magCal_temp, 6, "MAGCAL", ", ");
    }
    return newMax;
};

bool GY87::gyroCalibrationLoop(){
    float A_temp[3], G_temp[3], M_temp[3], B_temp;
    static builtin_led Led;
    static bool ledState = 0;

    for (int i = 0; i < 100; i++){
        accumulateData();
    }
    
    getData(A_temp, G_temp, M_temp, &B_temp);

    std::cout << G_temp[0] << ", ";
    std::cout << G_temp[1] << ", ";
    std::cout << G_temp[2] << ", ";
    std::cout << 1 << ", ";
    std::cout << 1 << ", ";
    std::cout << 1 << std::endl;

    if (ledState){
        ledState = 0;
    } else {
        ledState = 1;
    }
    Led.set_level(ledState);

    return 0;
}

bool GY87::magCalibrationLoop(){
    static float mag_param[6] = {0};
    static uint8_t count = 0;
    static builtin_led Led;

    if (calibrateMag(&mag_param[0])){
        count = 0;
    }

    if (count < 2){
        Led.set_level(1);
        count++;
    } else {
        Led.set_level(0);
    }
    return 0;
}

bool GY87::cleanAccum(){
    A_raw_accum[0] = 0;
    A_raw_accum[1] = 0;
    A_raw_accum[2] = 0;
    G_raw_accum[0] = 0;
    G_raw_accum[1] = 0;
    G_raw_accum[2] = 0;
    M_raw_accum[0] = 0;
    M_raw_accum[1] = 0;
    M_raw_accum[2] = 0;
    B_raw_accum = 0;

#if LOG_TIMER
    serialLogger::logUInt16(&N_samples, "NSMP");
#endif

    N_samples = 0;

    return 0;
}

GY87::GY87(){

    setup_i2c();

    // IMU
    opt = GY87_PWR_MGMT_1_OPT;
    write(&opt, 1, GY87_IMU_ADD, GY87_PWR_MGMT_1_ADD);

    // ---------------------------------------------
    opt = GY87_USER_CTRL_OPT_1;
    write(&opt, 1, GY87_IMU_ADD, GY87_USER_CTRL_ADD);

    opt = GY87_CONFIG_1_OPT_1;
    write(&opt, 1, GY87_IMU_ADD, GY87_CONFIG_1_ADD);

    // MAG
    opt = GY87_CONFIG_2_OPT;
    write(&opt, 1, GY87_MAG_ADD, GY87_CONFIG_2_ADD);

    opt = GY87_CONFIG_1_OPT_2;
    write(&opt, 1, GY87_IMU_ADD, GY87_CONFIG_1_ADD);

    opt = GY87_USER_CTRL_OPT_2;
    write(&opt, 1, GY87_IMU_ADD, GY87_USER_CTRL_ADD);

    // ---------------------------------------------
    // IMU should read MAG
    opt = GY87_CONFIG_3_OPT;
    write(&opt, 1, GY87_IMU_ADD, GY87_CONFIG_3_ADD);

    opt = GY87_CONFIG_4_OPT;
    write(&opt, 1, GY87_IMU_ADD, GY87_CONFIG_4_ADD);

    opt = GY87_CONFIG_5_OPT;
    write(&opt, 1, GY87_IMU_ADD, GY87_CONFIG_5_ADD);

    opt = GY87_CONFIG_6_OPT;
    write(&opt, 1, GY87_IMU_ADD, GY87_CONFIG_6_ADD);

    getBaroCal();
    setPressureMeas();
};

void GY87::getBaroCal(){
    read(buffer, 22, GY87_BARO_ADD, GY87_BARO_CALIB_REG);
    AC1 = (buffer[0] << 8 | buffer[1]);
    AC2 = (buffer[2] << 8 | buffer[3]);
    AC3 = (buffer[4] << 8 | buffer[5]);
    AC4 = (buffer[6] << 8 | buffer[7]);
    AC5 = (buffer[8] << 8 | buffer[9]);
    AC6 = (buffer[10] << 8 | buffer[11]);
    B1 = (buffer[12] << 8 | buffer[13]);
    B2 = (buffer[14] << 8 | buffer[15]);
    MB = (buffer[16] << 8 | buffer[17]);
    MC = (buffer[18] << 8 | buffer[19]);
    MD = (buffer[20] << 8 | buffer[21]);
}

void GY87::calcTruePressureAccum(){
    X1 = (T_raw - AC6) * AC5 / 32768;
    X2 = MC * 2048 / (X1 + MD);
    B5 = X1 + X2;
    T = (B5 + 8) / 16;

    B6 = B5 - 4000;
    X1 = (B2 * (B6 * B6 / 4096)) / 2048;
    X2 = AC2 * B6 / 2048;
    X3 = X1 + X2;
    B3 = (AC1 * 4 + X3 + 2) / 4;
    X1 = AC3 * B6 / 8192;
    X2 = (B1 * (B6 * B6 / 4096)) / 65536;
    X3 = (X1 + X2 + 2) / 4;
    B4 = AC4 * (uint32_t)(X3 + 32768) / 32768;
    B7 = ((uint32_t)(B_raw_accum/N_samples) - B3) * 50000;
    if (B7 < 0X80000000) {
        p = (B7 * 2) / B4;
    } else {
        p = (B7 / B4) * 2;
    }
    X1 = (p / 256) * (p / 256);
    X1 = (X1 * 3038) / 65536;
    X2 = (-7357 * p) / 65536;
    p = p + (X1 + X2 + 3791) / 16;
}

void GY87::calcTruePressure(){
    X1 = (T_raw - AC6) * AC5 / 32768;
    X2 = MC * 2048 / (X1 + MD);
    B5 = X1 + X2;
    T = (B5 + 8) / 16;

    B6 = B5 - 4000;
    X1 = (B2 * (B6 * B6 / 4096)) / 2048;
    X2 = AC2 * B6 / 2048;
    X3 = X1 + X2;
    B3 = (AC1 * 4 + X3 + 2) / 4;
    X1 = AC3 * B6 / 8192;
    X2 = (B1 * (B6 * B6 / 4096)) / 65536;
    X3 = (X1 + X2 + 2) / 4;
    B4 = AC4 * (uint32_t)(X3 + 32768) / 32768;
    B7 = ((uint32_t)(B_raw) - B3) * 50000;
    if (B7 < 0X80000000) {
        p = (B7 * 2) / B4;
    } else {
        p = (B7 / B4) * 2;
    }
    X1 = (p / 256) * (p / 256);
    X1 = (X1 * 3038) / 65536;
    X2 = (-7357 * p) / 65536;
    p = p + (X1 + X2 + 3791) / 16;
}


float GY87::calcAlt(){
    fbuffer = (float)p/101325.0f;
    fbuffer = 44330.0f * (1.0 - pow(fbuffer, 0.190295f));
    return fbuffer;
}

void GY87::setPressureMeas(){
    opt = GY87_CONFIG_10_OPT;
    write(&opt, 1, GY87_BARO_ADD, GY87_CONFIG_10_ADD);
}

void GY87::setTemperatureMeas(){
    opt = GY87_CONFIG_11_OPT;
    write(&opt, 1, GY87_BARO_ADD, GY87_CONFIG_11_ADD);
}

bool GY87::readData(){

    if (read(buffer, 20, GY87_IMU_ADD, GY87_IMU_DATA_ADD) != ESP_OK){
        return 1;
    }

    A_raw[0] = (buffer[0] << 8 | buffer[1]);
    A_raw[1] = (buffer[2] << 8 | buffer[3]);
    A_raw[2] = (buffer[4] << 8 | buffer[5]);

    G_raw[0] = (buffer[8] << 8 | buffer[9]);
    G_raw[1] = (buffer[10] << 8 | buffer[11]);
    G_raw[2] = (buffer[12] << 8 | buffer[13]);

    M_raw[0] = (buffer[15] << 8 | buffer[14]);
    M_raw[1] = (buffer[17] << 8 | buffer[16]);
    M_raw[2] = (buffer[19] << 8 | buffer[18]);

    switch (baroState) {
        case (SHOULD_READ_TEMP):
            setTemperatureMeas();
            baroState = WAIT_FOR_TEMP;
            break;

        case (WAIT_FOR_TEMP):
            baroState = READ_TEMP;
            break;

        case (READ_TEMP):
            read(buffer, 2, GY87_BARO_ADD, GY87_BARO_REG);
            T_raw = (buffer[0] << 8 | buffer[1]);
            baroState = SHOULD_READ_PRESSURE;
            break;

        case (SHOULD_READ_PRESSURE):
            setPressureMeas();
            baroState = WAIT_FOR_PRESSURE;
            break;

        case (WAIT_FOR_PRESSURE):
            baroState = READ_PRESSURE;
            break;

        case (READ_PRESSURE):
            read(buffer, 2, GY87_BARO_ADD, GY87_BARO_REG);
            B_raw = (buffer[0] << 8 | buffer[1]);
            baroState = SHOULD_READ_PRESSURE;
            break;
    }

    return 0;
}

bool GY87::setup_i2c(){
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = GY87_SDA_IO;
    conf.scl_io_num = GY87_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = GY87_MASTER_FREQ_HZ;
    conf.clk_flags = 0;

    i2c_param_config(GY87_I2C_PORT, &conf);
    i2c_driver_install(GY87_I2C_PORT, conf.mode, 0, 0, 0);
    return 0;
};

esp_err_t GY87::write(uint8_t* data_ptr, const size_t size, const uint8_t address, const uint8_t register_address){

    i2c_cmd_handle_t command = i2c_cmd_link_create();
    esp_err_t i2c_OK = ESP_OK;

    i2c_master_start(command);
    i2c_master_write_byte(command, (address << 1) | GY87_I2C_WRITE_BIT, GY87_I2C_ACK_CHECK_EN);
    i2c_master_write_byte(command, register_address, GY87_I2C_ACK_CHECK_EN);
    i2c_master_write(command, data_ptr, size, GY87_I2C_ACK_CHECK_EN);
    i2c_master_stop(command);

    i2c_OK = i2c_master_cmd_begin(GY87_I2C_PORT, command, 1000/portTICK_RATE_MS);
    i2c_cmd_link_delete(command);
    return i2c_OK;
};

esp_err_t GY87::read(uint8_t* buffer_ptr, const size_t size, const uint8_t address, const uint8_t register_address){

    i2c_cmd_handle_t command = i2c_cmd_link_create();
    esp_err_t i2c_OK = ESP_OK;

    i2c_master_start(command);
    i2c_master_write_byte(command, (address << 1) | GY87_I2C_WRITE_BIT, GY87_I2C_ACK_CHECK_EN);
    i2c_master_write_byte(command, register_address, GY87_I2C_ACK_CHECK_EN);

    i2c_master_start(command);
    i2c_master_write_byte(command, (address << 1) | GY87_I2C_READ_BIT, GY87_I2C_ACK_CHECK_EN);
    for (auto i = 0; i < size - 1; i++)
    {
        i2c_master_read_byte(command, buffer_ptr + i, GY87_I2C_ACK_VAL);
    }
    i2c_master_read_byte(command, buffer_ptr + size - 1, GY87_I2C_NACK_VAL);
    i2c_master_stop(command);

    i2c_OK = i2c_master_cmd_begin(GY87_I2C_PORT, command, 1000/portTICK_RATE_MS);
    i2c_cmd_link_delete(command);
    return i2c_OK;
};
