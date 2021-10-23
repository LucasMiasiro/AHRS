#include "GY87.h"
#include "driver/i2c.h"
#include "config.h"
#include "gpio-handler.h"
#include <math.h> 

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

    N_samples++;

    return 0;
}

bool GY87::getData(float* A, float* G, float* M){
    if (N_samples == 0){
        *A = A_raw[0]*GY87_ACCEL_SENS;
        *(A+1) = A_raw[1]*GY87_ACCEL_SENS;
        *(A+2) = A_raw[2]*GY87_ACCEL_SENS;

        *G = G_raw[0]*GY87_GYRO_SENS - gyroCal[0];
        *(G+1) = G_raw[1]*GY87_GYRO_SENS - gyroCal[1];
        *(G+2) = G_raw[2]*GY87_GYRO_SENS - gyroCal[2];

        *M = M_raw[0]*GY87_MAG_SENS - magCal[0];
        *(M+1) = M_raw[1]*GY87_MAG_SENS - magCal[1];
        *(M+2) = M_raw[2]*GY87_MAG_SENS - magCal[2];

        return 1;
    }

    *A = A_raw_accum[0]*GY87_ACCEL_SENS/N_samples;
    *(A+1) = A_raw_accum[1]*GY87_ACCEL_SENS/N_samples;
    *(A+2) = A_raw_accum[2]*GY87_ACCEL_SENS/N_samples;

    *G = G_raw_accum[0]*GY87_GYRO_SENS/N_samples - gyroCal[0];
    *(G+1) = G_raw_accum[1]*GY87_GYRO_SENS/N_samples - gyroCal[1];
    *(G+2) = G_raw_accum[2]*GY87_GYRO_SENS/N_samples - gyroCal[2];

    *M = M_raw_accum[0]*GY87_MAG_SENS/N_samples - magCal[0];
    *(M+1) = M_raw_accum[1]*GY87_MAG_SENS/N_samples - magCal[1];
    *(M+2) = M_raw_accum[2]*GY87_MAG_SENS/N_samples - magCal[2];

    magModule = sqrt((*M)*(*M) + (*(M+1))*(*(M+1)) + (*(M+2))*(*(M+2)));

    cleanAccum();

    return 0;
}

bool GY87::calibrateMag(float *mag_min_max){

    uint8_t buffer[20];
    float magCal_temp[6] = {0};
    bool newMax = 0;

    read(buffer, 20, GY87_IMU_ADD, GY87_IMU_DATA_ADD);
    int16_t mag1_raw = (buffer[15] << 8 | buffer[14]);
    int16_t mag2_raw = (buffer[17] << 8 | buffer[16]);
    int16_t mag3_raw = (buffer[19] << 8 | buffer[18]);
    float mag1 = (float)mag1_raw*GY87_MAG_SENS;
    float mag2 = (float)mag2_raw*GY87_MAG_SENS;
    float mag3 = (float)mag3_raw*GY87_MAG_SENS;
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
        magCal_temp[3] = 1;
        magCal_temp[4] = 1;
        magCal_temp[5] = 1;

        for (auto i = 0; i < 5; i++){
            std::cout << (float)*(magCal_temp+i) << ", ";
        }
        std::cout << (float)*(magCal_temp+5);
        std::cout << std::endl;
    }
    return newMax;
};

bool GY87::gyroCalibrationLoop(){
    float A_temp[3], G_temp[3], M_temp[3];

    for (int i = 0; i < 100; i++){
        accumulateData();
    }
    
    getData(A_temp, G_temp, M_temp);

    std::cout << G_temp[0] << ", ";
    std::cout << G_temp[1] << ", ";
    std::cout << G_temp[2] << ", ";
    std::cout << 1 << ", ";
    std::cout << 1 << ", ";
    std::cout << 1 << std::endl;
    return 0;
}

bool GY87::magCalibrationLoop(){
    static float mag_param[6] = {0};
    static uint8_t count = 0;
    static builtin_led Led;

    if (calibrateMag(&mag_param[0])){
        count = 0;
    }

    if (count < 10){
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

    // std::cout << "[samples] "<< (int)N_samples << std::endl;

    N_samples = 0;

    return 0;
}

GY87::GY87(){
    uint8_t opt;

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

    // IMU should read MAG
    opt = GY87_CONFIG_3_OPT;
    write(&opt, 1, GY87_IMU_ADD, GY87_CONFIG_3_ADD);

    opt = GY87_CONFIG_4_OPT;
    write(&opt, 1, GY87_IMU_ADD, GY87_CONFIG_4_ADD);

    opt = GY87_CONFIG_5_OPT;
    write(&opt, 1, GY87_IMU_ADD, GY87_CONFIG_5_ADD);

    opt = GY87_CONFIG_6_OPT;
    write(&opt, 1, GY87_IMU_ADD, GY87_CONFIG_6_ADD);
};

bool GY87::readData(){
    uint8_t buffer[20];
    esp_err_t read_OK = read(buffer, 20, GY87_IMU_ADD, GY87_IMU_DATA_ADD);

    if (read_OK != ESP_OK){
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
