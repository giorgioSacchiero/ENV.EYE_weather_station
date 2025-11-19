/* ------------------------- C++ (.cpp) FILE INFORMATION ------------------------- */
/**
 * PROJECT:         ENV.EYE SPOT Project
 * 
 * FILE:            "env_eye_lora.cpp"
 * 
 */

#include "inttypes.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "sdkconfig.h"

#include "env_eye_pinout.h"
#include "lora.h"
#include "env_eye_lora.hpp"

#define TAG "LORA_CPP"

static int16_t _prev_lora_lost = 0;

esp_err_t lora_configure(uint64_t _spi_freq, uint64_t _lora_freq, uint8_t _bw, uint8_t _sf, uint8_t _cr, bool _crc_enable)
{
    //initialize module with provided configuration
    ESP_LOGI(TAG, "Pinout: SPI_CLK: %d\tSPI_MISO: %d\tSPI_MOSI: %d\tSPI_CS: %d\tRESET: %d", CONFIG_SCK_GPIO, CONFIG_MISO_GPIO, CONFIG_MOSI_GPIO, CONFIG_CS_GPIO, CONFIG_RST_GPIO);

    if(int _lora_err = lora_init(_spi_freq) == 0)
    {
        ESP_LOGE(TAG, "LoRa module initialization failed. Error code %d", _lora_err);
        return ESP_FAIL;
    }

    //set desired frequency value
    lora_set_frequency(_lora_freq);

    //Set communication parameters
    lora_set_bandwidth(_bw);
    if(int _read = lora_get_bandwidth() != _bw)
    {
        ESP_LOGE(TAG, "Error in bandwith setup. Provided %d\t Read %d.", _bw, _read);
        return ESP_FAIL;
    }

    lora_set_spreading_factor(_sf);
    if(int _read = lora_get_spreading_factor() != _sf)
    {
        ESP_LOGE(TAG, "Error in bandwith setup. Provided %d\t Read %d.", _sf, _read);
        return ESP_FAIL;
    }

    lora_set_coding_rate(_cr);
    if(int _read = lora_get_coding_rate() != _cr)
    {
        ESP_LOGE(TAG, "Error in bandwith setup. Provided %d\t Read %d.", _cr, _read);
        return ESP_FAIL;
    }
    if(_crc_enable) lora_enable_crc();
    else lora_disable_crc();

    return ESP_OK;
}

esp_err_t lora_send_measurement_data(lora_packet_t *_packet)
{
    //send data through LoRa
    lora_send_packet(_packet->raw_payload, sizeof(_packet->raw_payload));
    int16_t _lost = lora_packet_lost();
    if(_lost != _prev_lora_lost)
    {    
        ESP_LOGE(TAG, "Error while sending data. Lost %d packets.", _lost);
        _prev_lora_lost = _lost;
        return ESP_FAIL;
    }

    return ESP_OK;
}

void lora_init_payload(lora_payload_t *_payload)
{
       _payload->temperature = -99;
       _payload->humidity = -99;
       _payload->air_pressure = -99;
       _payload->air_pollution_level = -99;
       _payload->wind_spd = -99;
       _payload->wind_dir = -99;
       _payload->noise_level = -99;
       _payload->radiation_level = -99;
}