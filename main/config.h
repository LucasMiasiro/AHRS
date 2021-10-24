#pragma once

// Filter Control
#define GYRO_ERROR                      2
#define GYRO_DRIFT                      0.2
#define COMPENSATE                      1
#define WMM_LOCALFIELD                  0.8660f, 0.0f, 0.5f
#define R_0                             0.0f, 0.0f, 1.0f
#define ANG_0                           0.0f

// Application Control
#define SYSTEM_SAMPLE_PERIOD_MS         100
#define DEFAULT_MODE                    1
#if DEFAULT_MODE
#define CALIBRATE_MAG                   0
#define CALIBRATE_GYRO                  0
#else
#define CALIBRATE_MAG                   1
#define CALIBRATE_GYRO                  0
#endif

#define SHOULD_LOG                      1
#if SHOULD_LOG
    #define LOG_MAIN                    1
    #define LOG_GY87                    0
    #define LOG_DCM                     0
    #define LOG_TIMER                   0
    #define SEND_SERIAL                 1
#else
    #define LOG_MAIN                    0
    #define LOG_GY87                    0
    #define LOG_DCM                     0
    #define LOG_TIMER                   0
    #define SEND_SERIAL                 0
#endif

// _________________________________________________________________________

// I2C Param
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

// IMU
#define GY87_IMU_ADD                    0X68
#define GY87_PWR_MGMT_1_ADD             0X6B
#define GY87_PWR_MGMT_1_OPT             0X00 // Disable sleep
#define GY87_USER_CTRL_ADD              0X6A
#define GY87_USER_CTRL_OPT_1            0X00 // Disable master mode
#define GY87_USER_CTRL_OPT_2            0X20 // Enable master mode
#define GY87_CONFIG_1_ADD               0X37
#define GY87_CONFIG_1_OPT_1             0X02 // Enable master bypass mode
#define GY87_CONFIG_1_OPT_2             0X00 // Disable master bypass mode
#define GY87_CONFIG_3_ADD               0X25
#define GY87_CONFIG_3_OPT               GY87_MAG_ADD | 0X80 // Read from MAG
#define GY87_CONFIG_4_ADD               0X26
#define GY87_CONFIG_4_OPT               0X00 // Data register
#define GY87_CONFIG_5_ADD               0X27
#define GY87_CONFIG_5_OPT               6 | 0X80 // Transfer size
#define GY87_CONFIG_6_ADD               0X67
#define GY87_CONFIG_6_OPT               1 // 0 delay
#define GY87_IMU_DATA_ADD               0X3B // First accel data address
#define GY87_ACCEL_SENS                 1.0/16384/1.05f // Accel sensivity
#define GY87_GYRO_SENS                  1.0/131.0*DEG2RAD // Gyro sensivity
#if CALIBRATE_GYRO
    #define GY87_GYRO_CAL                0, 0, 0, 1, 1, 1
#else
    #define GY87_GYRO_CAL                -0.0304047, 0.00966193, 0.0254938, 1, 1, 1
#endif

// MAG
#define GY87_MAG_ADD                    0X0D
#define GY87_CONFIG_2_ADD               0X09
#define GY87_CONFIG_2_OPT               0XCD // Continuous Measurement Mode, Output Rate, Range, Oversampling Rate
#define GY87_MAG_SENS                   0.0833333333f // Mag sensivity
#if CALIBRATE_MAG
    #define GY87_MAG_CAL                0, 0, 0, 1, 1, 1
#else
    #define GY87_MAG_CAL                -74.1667, -188.833, -108.958, 1.02326, 1.01448, 0.963317
#endif

// Constants
#define GY87_TEMP_SENS                  1 // Temp sensivity
#define GRAVITY                         9.80665 // Gravity
#define DEG2RAD                         0.0174532925f //PI/180

// GPIO
#define BUILTIN_LED                     GPIO_NUM_2
