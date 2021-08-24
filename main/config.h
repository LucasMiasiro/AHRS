#pragma once

#define SHOULD_LOG                      1

#if SHOULD_LOG
    #include <iostream>
    #define LOG_MAIN                    1
    #define LOG_GY87                    1
#else
    #define LOG_MAIN                    0
    #define LOG_GY87                    0
#endif

#define GY87_SDA_IO                     21
#define GY87_SCL_IO                     22
#define GY87_MASTER_FREQ_HZ             100000
#define GY87_I2C_PORT                   I2C_NUM_1
#define GY87_I2C_WRITE_BIT              I2C_MASTER_WRITE
#define GY87_I2C_READ_BIT               I2C_MASTER_READ
#define GY87_I2C_ACK_CHECK_EN	        0x1
#define GY87_I2C_ACK_CHECK_DIS	        0x0
#define GY87_I2C_ACK_VAL	            I2C_MASTER_ACK
#define GY87_I2C_NACK_VAL	            I2C_MASTER_NACK

#define GY87_IMU_ADD                    0X68
#define GY87_PWR_MGMT_1_ADD             0X6B // Power management address
#define GY87_PWR_MGMT_1_OPT             0X00 // Power management option
#define GY87_DATA_ADD                   0X3B // First accel data address
#define GY87_ACCEL_SENS                 16384.0 // Accel sensivity
#define GY87_TEMP_SENS                  1 // Temp sensivity
#define GY87_GYRO_SENS                  131.0 // Gyro sensivity
#define GY87_MAG_SENS                   12 // Mag sensivity

// #define INT_PIN_CFG 0X37
// #define USER_CTRL 0X6A
// #define GY87_GYRO 0X43 // First gyro data address

// #define MAG_ADD 0X0D
// #define MODE_REG 9
// #define MAG 01

#define GRAVITY                         9.80665 // Gravity