#pragma once
#pragma pack(1)
/* ------------------------- HEADER (.hpp) FILE INFORMATION ------------------------- */
/**
 * PROJECT:         ENV.EYE SPOT Project
 * 
 * FILE:            "env_eye_lora.hpp"
 * DESCRIPTION:     LoRa communication functions.
 * 
 */

#include "inttypes.h"
#include "esp_err.h"

typedef union
{
    uint8_t raw_payload[24];
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
    }data;
}lora_packet_t;

typedef struct
{
    uint8_t module_id = 0;
    uint8_t wakeup_num = 0;
    float temperature = -1;
    float humidity = -1;              //% * 100
    float air_pressure = -1;          //hPa * 100
    float air_pollution_level = -1;   //mg/m^3 * 100
    float wind_spd = -1;              //m/s * 100
    int16_t wind_dir = -1;            //1-8
    float noise_level = -1;           //dB * 100
    float radiation_level = -1;       //uSv/h * 100
}lora_payload_t;


 /**
 * @brief Configure LoRa module interface and communication parameters.
 * 
 * @param _spi_freq SPI clock frequency (SCK PIN)
 * @param _lora_freq LoRa protocol carrier frequency
 * @param _bw LoRa protocol bandwith
 * @param _sf LoRa protol spreading factor
 * @param _cr LoRa protocol coding rate
 * @param _crc_enable Enable/Disable CRC in LoRa communication
 * 
 * @returns ESP_OK on success.
 */
esp_err_t lora_configure(uint64_t _spi_freq, uint64_t _lora_freq, uint8_t _bw, uint8_t _sf, uint8_t _cr, bool _crc_enable);

 /**
 * @brief Send a measurement packet.
 * 
 * @param _packet Pointer to lora_packet_t data to be sent.
 * 
 * @returns ESP_OK on success.
 */
esp_err_t lora_send_measurement_data(lora_packet_t *_packet);

/**
 * @brief Initialize lora_payload_t structure to default values
 * 
 * @param _payload Pointer to lora_payload_t data to be initialized.
 * 
 * @returns void.
 */
void lora_init_payload(lora_payload_t *_payload);

