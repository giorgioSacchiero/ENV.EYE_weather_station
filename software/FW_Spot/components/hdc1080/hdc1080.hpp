#pragma once
/* ------------------------- HEADER (.h) FILE INFORMATION ------------------------- */
/**
 * PROJECT:         ENV.EYE SPOT Project
 * 
 * FILE:            "hdc1080.hpp"
 * DESCRIPTION:     HDC1080 Temperature and Humidity sensor API.
 * 
 */

#include "inttypes.h"
#include "driver/i2c_master.h"

typedef union 
{
    uint8_t hdc1080_config_value;
    struct 
    {
        uint8_t humidity_measurement_resolution : 2;
		uint8_t temperature_measurement_resolution : 1;
		uint8_t battery_status : 1;
		uint8_t mode_of_acquisition : 1;
		uint8_t heater : 1;
		uint8_t reserved_bit : 1;
		uint8_t software_reset : 1;
    }flags;
}hdc1080_config_t;

class HDC1080
{
    public:
    HDC1080(gpio_num_t _sda_pin, gpio_num_t _scl_pin, uint32_t _freq, i2c_port_t _i2c_port_num, bool _init_bus);
    HDC1080(gpio_num_t _sda_pin, gpio_num_t _scl_pin, uint32_t _freq, i2c_port_t _i2c_port_num, uint32_t _timeout_ms, bool _init_bus);
    esp_err_t configure(hdc1080_config_t _config);
    esp_err_t configureDefault();

    esp_err_t measure();
    float getTemp();
    float getHum();

    private:
    esp_err_t _checkDevManID();
    esp_err_t _writeReg(uint8_t _reg_num, void *_write_data, uint8_t _size);
    esp_err_t _writeReadReg(uint8_t _reg_num, void *_read_data, uint8_t _size);
    esp_err_t _readReg(void *_read_data, uint8_t _size);

    i2c_master_bus_handle_t _bus_handle;
    i2c_master_dev_handle_t _dev_handle;
    uint32_t _tout_ms = 10;

    uint64_t _latest_measure_time = 0;
    float _latest_temp = -1, _latest_hum = -1;
};