#pragma once
#pragma pack(1)
/* ------------------------- HEADER (.h) FILE INFORMATION ------------------------- */
/**
 * PROJECT:         ENV.EYE HUB Project
 * 
 * FILE:            "lora_conf.h"
 * DESCRIPTION:     LoRa Communication configuration parameters and functions.
 * 
*/

/* ------------------------- MACRO DEFINITION -------------------------*/
#define LORA_COMM_FREQ 868E6
#define LORA_SPI_FREQ 100000
#define LORA_CS_PIN 10
#define LORA_RESET_PIN 7
#define LORA_INTR_PIN -1
#define LORA_BANDWIDTH 125E3
#define LORA_SPREADING_FACTOR 7
#define LORA_CODING_RATE 1

#define LORA_ENABLE_CRC


/* ------------------------- VARIABLES AND DATATYPES ------------------------- */
typedef union
{
    uint8_t raw_payload[22];
    struct data_str
    {
        uint8_t module_id;
        uint8_t wakeup_num;            //Number of consecutive wakeups
        int16_t temperature;           //°C * 100
        int16_t humidity;              //% * 100
        int32_t air_pressure;          //hPa * 100
        int16_t air_pollution_level;   //mg/m^3 * 100
        int16_t wind_spd;              //m/s * 100
        int16_t wind_dir;              //1-8
        int16_t noise_level;           //dB * 100
        int16_t radiation_level;       //uSv/h * 100
        
        int16_t signal_power;          //dB
    }data;
}lora_packet_t;

/* ------------------------- FUNCTIONS ------------------------- */
void lora_begin();
void lora_update();

//Returns true if packet is received
bool lora_receive(lora_packet_t* _data);