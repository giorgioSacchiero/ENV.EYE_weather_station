/* ------------------------- HEADER (.h) FILE INFORMATION ------------------------- */
/**
 * PROJECT:         ENV.EYE SPOT Project
 * 
 * FILE:            "hdc1080_registers.h"
 * DESCRIPTION:     HDC1080 Temperature and Humidity sensor register codes. 
 * 
 */

/* ------------------------- Register Codes ------------------------- */
#define HDC1080_TEMPERATURE_REG     0x00    /* TEMPERATURE MEASUREMENT OUTPUT */
#define HDC1080_HUMIDITY_REG        0x01    /* RELATIVE HUMIDITY MEASUREMENT OUTPUT */
#define HDC1080_CONFIG_REG          0x02    /* HDC1080 CONFIGURATION DATA */
#define HDC1080_SERIALID2_REG       0xFB    /* FIRST 2 BYTES OF SERIAL ID */
#define HDC1080_SERIALID1_REG       0xFC    /* MID 2 BYTES OF THE SERIAL ID */
#define HDC1080_SERIALID0_REG       0xFD    /* LAST BYTE BIT OF THE SERIAL ID */
#define HDC1080_MANUFACTURER_ID_REG 0xFE    /* ID OF TEXAS INSTRUMENTS */
#define HDC1080_DEVICE_ID_REG       0xFF    /* REGISTER OF THE DEVICE ID */
#define HDC1080_DEVICE_ID           0x1050  /* HDC1080 UNIQUE ID */
#define HDC1080_MANUFACTURER_ID     0x5449  /* TI MANUFACTURER ID */
#define HDC1080_I2C_ADDRESS         0x40    /* I2C ADDRESS OF THE HDC1080 */

/* ------------------------- Configuration Options ------------------------- */
#define HDC1080_ACQUISITION_HUMIDITY_AND_TEMPERATURE  0x01
#define HDC1080_ACQUISITION_HUMIDITY_OR_TEMPERATURE   0x00
#define HDC1080_TEMPERATURE_RESOLUTION_11BIT          0x01
#define HDC1080_TEMPERATURE_RESOLUTION_14BIT          0x00
#define HDC1080_HUMIDITY_RESOLUTION_8BIT              0x02
#define HDC1080_HUMIDITY_RESOLUTION_11BIT             0x01
#define HDC1080_HUMIDITY_RESOLUTION_14BIT             0x00

#define HDC1080_HEATER_ENABLED      0x01
#define HDC1080_HEATER_DISABLED     0x00
#define HDC1080_BATTERY_STATUS_OK   0x00
#define HDC1080_BATTERY_STATUS_LOW  0x01
#define HDC1080_ERR_ID              0xFF
#define HDC1080_CONVERTING          0xFE

