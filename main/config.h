#pragma once

// Filter Control
#define GYRO_ERROR                      1.0
#define GYRO_DRIFT                      0.2
#define COMPENSATE                      1
#define WMM_LOCALFIELD                  0.8660f, 0.0f, 0.5f
#define WMM_MAG_DECLINATION             -20.2*DEG2RAD
#define R_0                             0.0f, 0.0f, 1.0f
#define ANG_0                           0.0f
#define AXIS_CONFIG                     0
#define GNSS_FUSION_MIN_VEL             1.0f

// Application Control
#define SYSTEM_SAMPLE_PERIOD_MS         100
#define APP_MODE                        0

#define SHOULD_LOG                      1
#if SHOULD_LOG
    #define LOG_MAIN                    1
    #define LOG_GY87                    0
    #define LOG_DCM                     0
    #define LOG_TIMER                   0
    #define LOG_ATGM336                 0
    #define LOG_NEOM8N                  0
    #define SEND_MODE                   1
    #define LOG_SD                      0
#else
    #define LOG_MAIN                    0
    #define LOG_GY87                    0
    #define LOG_DCM                     0
    #define LOG_TIMER                   0
    #define LOG_ATGM336                 0
    #define LOG_NEOM8N                  0
    #define SEND_MODE                   0
    #define LOG_SD                      0
#endif

#define SHOULD_USE_MAG2                 1

// _________________________________________________________________________

// GPIO and I2C Parameters
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

// Mag as slave
#define GY87_CONFIG_3_ADD               0X25 // Setup Slave0 Device Address
#define GY87_CONFIG_3_OPT               GY87_MAG_ADD | 0X80 // Read from MAG
#define GY87_CONFIG_4_ADD               0X26 // Setup Slave0 First Register
#define GY87_CONFIG_4_OPT               0X00 // Data register
#define GY87_CONFIG_5_ADD               0X27
#define GY87_CONFIG_5_OPT               6 | 0X80 // Transfer size
#define GY87_CONFIG_6_ADD               0X67
#define GY87_CONFIG_6_OPT               2 | 1 // 0 delay for Mag and Baro

#define GY87_CONFIG_10_ADD              0XF4
#define GY87_CONFIG_10_OPT              0X34
#define GY87_CONFIG_11_ADD              0XF4
#define GY87_CONFIG_11_OPT              0X2E

#define GY87_IMU_DATA_ADD               0X3B // First accel data address
#define GY87_ACCEL_SENS                 -1.0/16384/1.05f // Accel sensivity
#define GY87_GYRO_SENS                  1.0/131.0*DEG2RAD // Gyro sensivity
#define GY87_TEMP_SENS                  1 // Temp sensivity
#if APP_MODE==2
    #define GY87_GYRO_CAL                0, 0, 0, 1, 1, 1
#else
    #define GY87_GYRO_CAL                -0.0305366, 0.00793925, 0.0266915, 1, 1, 1
#endif

// MAG
#define GY87_MAG_ADD                    0X0D
#define GY87_CONFIG_2_ADD               0X09
#define GY87_CONFIG_2_OPT               0XCD // Continuous Measurement Mode, Output Rate, Range, Oversampling Rate

//MAG2
#define LSM_MAG_ADD                     0X1E
#define LSM_MAG_DATA_ADD                0X03
#define LSM_CONFIG_1_ADD                0X00
#define LSM_CONFIG_1_OPT                0X98 // Temp Enabled, min 75Hz Mag Output
#define LSM_CONFIG_2_ADD                0X01
#define LSM_CONFIG_2_OPT                0X40 // +-1.9 Gauss, sens 855 LSB/Gauss
#define LSM_CONFIG_3_ADD                0X02
#define LSM_CONFIG_3_OPT                0X00 // Continous-conversion mode

#define GY87_MAG_SENS                   0.0833333333f // Mag sensivity
#define LSM_MAG_SENS                    1.1695906433f // Mag2 sensivity
#if APP_MODE==1
    #define GY87_MAG_CAL                0, 0, 0, 1, 1, 1
#else
    #define GY87_MAG_CAL                -32.3567, -157.93, -54.5263, 0.893103, 0.998468, 1.12141
#endif

// BARO
#define GY87_BARO_ADD                   0X77
#define GY87_BARO_REG                   0XF6
#define GY87_BARO_CALIB_REG             0XAA

// Constants
#define GRAVITY                         9.80665
#define DEG2RAD                         0.0174532925f // pi/180
#define EARTH_FLATTENING                0.00335281f
#define EARTH_EQUATORIAL_RADIUS         6378137.0f
#define PI                              3.14159265359f

// Bluetooth
#define SPP_TAG                         "INS"
#define SPP_SERVER_NAME                 "INS_SERVER"
#define BT_INIT_MSG                     "Welcome\n"
#define BT_RECEIVED_MSG                 "Received: "
#define BT_DEVICE_NAME                  "Embedded Navigation System"
#define BT_SEND_MSG_EULER               "E"
#define BT_SEND_MSG_MAG                 "M"
#define BT_BUFFERSIZE                   20

// GNSS
#define GNSS_TASK_KB                    4
#define GNSS_TASK_CORE                  1
#define GNSS_TASK_PRIORITY              1
#define GNSS_MIN_SATS                   3
#define GNSS_RX_IO                      16
#define GNSS_BAUD                       9600
/* #define GNSS_CONV_TIME_S                20 */
#define GNSS_CONV_TIME_S                10
#define GNSS_CONV_VEL                   0.5f
#define GNSS_HOME_REQ_3D                0

// SD SPI
#define SD_MOUNT_POINT                  "/sdcard"
#define SD_MISO_IO                      12
#define SD_MOSI_IO                      13
#define SD_CLK_IO                       14
#define SD_CS_IO                        15
#define SD_MAX_SEARCH_LEN               10
#define SD_BUFFERSIZE                   20
#define SD_MAX_FREQ_HZ                  10000
#define SD_WRITE_UNTIL_SAVE             10
