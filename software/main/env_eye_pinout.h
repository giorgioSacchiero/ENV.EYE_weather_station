#pragma once
/* ------------------------- HEADER (.h) FILE INFORMATION ------------------------- */
/**
 * PROJECT:         ENV.EYE SPOT Project
 * 
 * FILE:            "env_eye_pinout.h"
 * DESCRIPTION:     REV0 and REV1 board pinouts
 * 
 */

#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"

/* ------------------------- REV0 Pinout ------------------------- */
struct 
{
    //SPI Protocol
    const gpio_num_t SPI_MOSI = GPIO_NUM_38;
    const gpio_num_t SPI_MISO = GPIO_NUM_47;
    const gpio_num_t SPI_CS = GPIO_NUM_21;
    const gpio_num_t SPI_SCK = GPIO_NUM_48;

    //I2C Protocol
    const gpio_num_t I2C_SDA = GPIO_NUM_11;
    const gpio_num_t I2C_SCL = GPIO_NUM_12;

    //UART Protocol
    const gpio_num_t UART_TX = GPIO_NUM_13;
    const gpio_num_t UART_RX = GPIO_NUM_14;
    
    //Rail Switching
    const gpio_num_t EN_3V3_RAIL = GPIO_NUM_6;
    const gpio_num_t EN_25V_RAIL = GPIO_NUM_7;
    const gpio_num_t EN_5V_GEIGER_RAIL = GPIO_NUM_8;
    const gpio_num_t EN_5V_SENSOR_RAIL = GPIO_NUM_9;
    const gpio_num_t EN_SERVO_RAIL = GPIO_NUM_18;
    
    //Digital Inputs
    const gpio_num_t CAP_VOLTAGE_OK = GPIO_NUM_17;
    const gpio_num_t GEIGER_IN = GPIO_NUM_5;

    //Digital Outputs
    const gpio_num_t SERVO_CONTROL = GPIO_NUM_1;
    const gpio_num_t LORA_RESET = GPIO_NUM_10;
    
    //Analog Inputs
    const adc_channel_t MIC_IN = ADC_CHANNEL_3;  //GPIO_NUM_4
    const adc_channel_t WIND_SPEED_IN = ADC_CHANNEL_2; //GPIO_NUM_3
    const adc_channel_t WIND_DIR_IN =  ADC_CHANNEL_1; //GPIO_NUM_2
}rev0_pinout;

/* ------------------------- REV1 Pinout ------------------------- */
struct
{
    //SPI Protocol
    const gpio_num_t SPI_MOSI = GPIO_NUM_38;
    const gpio_num_t SPI_MISO = GPIO_NUM_47;
    const gpio_num_t SPI_CS = GPIO_NUM_21;
    const gpio_num_t SPI_SCK = GPIO_NUM_48;

    //I2C Protocol
    const gpio_num_t I2C_SDA = GPIO_NUM_11;
    const gpio_num_t I2C_SCL = GPIO_NUM_12;

    //UART Protocol
    const gpio_num_t UART_TX = GPIO_NUM_14;
    const gpio_num_t UART_RX = GPIO_NUM_13;
    
    //Rail Switching
    const gpio_num_t EN_3V3_RAIL = GPIO_NUM_5;
    const gpio_num_t EN_25V_RAIL = GPIO_NUM_6;
    const gpio_num_t EN_5V_GEIGER_RAIL = GPIO_NUM_9;
    const gpio_num_t EN_5V_SENSOR_RAIL = GPIO_NUM_10;
    const gpio_num_t EN_SERVO_RAIL = GPIO_NUM_8;
    
    //Digital Inputs
    const gpio_num_t CAP_VOLTAGE_OK = GPIO_NUM_17;
    const gpio_num_t GEIGER_IN = GPIO_NUM_18;

    //Digital Outputs
    const gpio_num_t SERVO_CONTROL = GPIO_NUM_7;
    const gpio_num_t LORA_RESET = GPIO_NUM_3;
    
    //Analog Inputs
    const adc_channel_t MIC_IN = ADC_CHANNEL_3; //GPIO_NUM_4
    const adc_channel_t WIND_SPEED_IN = ADC_CHANNEL_0; //GPIO_NUM_1
    const adc_channel_t WIND_DIR_IN = ADC_CHANNEL_1; //GPIO_NUM_2
}rev1_pinout;