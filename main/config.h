#pragma once

// Filter Control
#define GYRO_ERROR                      2
#define GYRO_DRIFT                      0.2
#define COMPENSATE                      1
#define WMM_LOCALFIELD                  0.8660f, 0.0f, 0.5f
#define R_0                             0.0f, 0.0f, 1.0f
#define ANG_0                           0.0f
#define AXIS_CONFIG                     0

// Application Control
#define SYSTEM_SAMPLE_PERIOD_MS         100
#define APP_MODE                        0

#define SHOULD_LOG                      1
#if SHOULD_LOG
    #define LOG_MAIN                    1
    #define LOG_GY87                    0
    #define LOG_DCM                     0
    #define LOG_TIMER                   0
    #define LOG_ATGM336                 1
    #define SEND_MODE                   0
#else
    #define LOG_MAIN                    0
    #define LOG_GY87                    0
    #define LOG_DCM                     0
    #define LOG_TIMER                   0
    #define LOG_ATGM336                 0
    #define SEND_MODE                   0
#endif

// _________________________________________________________________________

// GIOP and I2C Param
#define BUILTIN_LED                     GPIO_NUM_2
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
#define GY87_ACCEL_SENS                 -1.0/16384/1.05f // Accel sensivity
#define GY87_GYRO_SENS                  1.0/131.0*DEG2RAD // Gyro sensivity
#define GY87_TEMP_SENS                  1 // Temp sensivity
#if APP_MODE==2
    #define GY87_GYRO_CAL                0, 0, 0, 1, 1, 1
#else
    #define GY87_GYRO_CAL                -0.0304047, 0.00966193, 0.0254938, 1, 1, 1
#endif

// MAG
#define GY87_MAG_ADD                    0X0D
#define GY87_CONFIG_2_ADD               0X09
#define GY87_CONFIG_2_OPT               0XCD // Continuous Measurement Mode, Output Rate, Range, Oversampling Rate
#define GY87_MAG_SENS                   0.0833333333f // Mag sensivity
#if APP_MODE==1
    #define GY87_MAG_CAL                0, 0, 0, 1, 1, 1
#else
    #define GY87_MAG_CAL                526.477, 559.228, 270.432, 1.1153, 0.98743, 0.908038
#endif

// Constants
#define GRAVITY                         9.80665 // Gravity
#define DEG2RAD                         0.0174532925f //PI/180

// BLUETOOTH
#define SPP_TAG             "INS"
#define SPP_SERVER_NAME     "INS_SERVER"
#define BT_INIT_MSG         "Welcome\n"
#define BT_RECEIVED_MSG     "Received: "
#define BT_DEVICE_NAME      "Embedded Navigation System"
#define BT_SEND_MSG_EULER   "E"
#define BT_SEND_MSG_MAG     "M"
#define BT_BUFFERSIZE       20

// GNSS
#define GNSS_TASK_KB        4
#define GNSS_TASK_CORE      1
#define GNSS_TASK_PRIORITY  1
#define GNSS_RX_IO          16
#define GNSS_BAUD           9600
