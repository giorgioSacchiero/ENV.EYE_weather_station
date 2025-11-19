/* ------------------------- C++ (.cpp) FILE INFORMATION ------------------------- */
/**
 * PROJECT:         ENV.EYE SPOT Project
 * 
 * FILE:            "hdc1080.cpp"
 * 
 */

#include "esp_log.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "memory.h"

#include "hdc1080.hpp"
#include "hdc1080_registers.h"

#define TAG "HDC1080"
#define HDC1080_MIN_SAMPLE_TIME_MS 500

#define ERR_LOG(FUNCT)  \
if ( esp_err_t _ERR = FUNCT != ESP_OK ) \
    ESP_LOGE(TAG, "ERROR (%d) at line %d. Description: %s", _ERR, __LINE__, esp_err_to_name(_ERR));

#define ERR_LOG_RET(FUNCT)  \
if ( esp_err_t _ERR = FUNCT != ESP_OK ) \
{   \
    ESP_LOGE(TAG, "ERROR (%d) at line %d. Description: %s", _ERR, __LINE__, esp_err_to_name(_ERR)); \
    return; \
}

#define ERR_LOG_RET_ERR(FUNCT)  \
if ( esp_err_t _ERR = FUNCT != ESP_OK ) \
{   \
    ESP_LOGE(TAG, "ERROR (%d) at line %d. Description: %s", _ERR, __LINE__, esp_err_to_name(_ERR)); \
    return _ERR; \
}

esp_err_t HDC1080::_writeReg(uint8_t _reg_num, void *_write_data, uint8_t _size)
{
    uint8_t _write_buf[1+_size];
    _write_buf[0] = _reg_num;

    if(_size)
        memcpy(_write_buf+1, _write_data, _size);
    
    ERR_LOG_RET_ERR(i2c_master_transmit(_dev_handle, _write_buf, sizeof(_write_buf), _tout_ms));
    return ESP_OK;
}

esp_err_t HDC1080::_writeReadReg(uint8_t _reg_num, void *_read_data, uint8_t _size)
{
    uint8_t _write_buf = _reg_num;
    ERR_LOG_RET_ERR(i2c_master_transmit_receive(_dev_handle, &_write_buf, sizeof(_write_buf), (uint8_t*)_read_data, _size, _tout_ms));
    return ESP_OK;
}

esp_err_t HDC1080::_readReg(void *_read_data, uint8_t _size)
{
    ERR_LOG_RET_ERR(i2c_master_receive(_dev_handle, (uint8_t*)_read_data, _size, _tout_ms));
    return ESP_OK;   
}

esp_err_t HDC1080::_checkDevManID()
{
    uint8_t _id_raw[2];
    ERR_LOG_RET_ERR(_writeReadReg(HDC1080_MANUFACTURER_ID_REG, &_id_raw, sizeof(_id_raw)));
    uint16_t _id_man = ((uint16_t)_id_raw[0]<<8) + _id_raw[1];
    
    ERR_LOG_RET_ERR(_writeReadReg(HDC1080_DEVICE_ID_REG, &_id_raw, sizeof(_id_raw)));
    uint16_t _id_dev = ((uint16_t)_id_raw[0]<<8) + _id_raw[1];

    if(_id_man != HDC1080_MANUFACTURER_ID)
    {
        ESP_LOGE(TAG, "Manufacturer ID doens't match, aborting configuration. \r\n\t\t Read: (%x) \t Expected (%x)", _id_man, HDC1080_MANUFACTURER_ID);
        return ESP_ERR_NOT_SUPPORTED;
    }

    if(_id_dev != HDC1080_DEVICE_ID)
    {
        ESP_LOGE(TAG, "Device ID doens't match, aborting configuration. \r\n\t\t Read: (%x) \t Expected (%x)", _id_dev, HDC1080_DEVICE_ID);
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}

/**
 * @brief Class constructor.
 * 
 * @param _sda_pin GPIO number of sda pin.
 * @param _scl_pin GPIO numbero of scl pin.
 * @param _freq I2C communication frequency
 * @param _i2c_port_num Internal I2C peripheral number.
 * @param _init_bus Initialize the I2C peripheral with the provided parameters.
 * 
 * @returns A new HDC1080 istance.
 */
HDC1080::HDC1080(gpio_num_t _sda_pin, gpio_num_t _scl_pin, uint32_t _freq, i2c_port_t _i2c_port_num, bool _init_bus)
{
    //Setup I2C Communication
    memset(&_bus_handle, 0, sizeof(_bus_handle));
    memset(&_dev_handle, 0, sizeof(_dev_handle));

    if(_init_bus)
    {
        i2c_master_bus_config_t _bus_config;
        memset(&_bus_config, 0, sizeof(_bus_config));
        _bus_config.i2c_port =  _i2c_port_num;
        _bus_config.sda_io_num = _sda_pin;
        _bus_config.scl_io_num = _scl_pin;
        _bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
        _bus_config.glitch_ignore_cnt = 7;
        _bus_config.intr_priority = 0;
        _bus_config.flags.enable_internal_pullup = 1;
        _bus_config.flags.allow_pd = 0;
        ERR_LOG_RET(i2c_new_master_bus(&_bus_config, &_bus_handle));
    }
    else ERR_LOG_RET(i2c_master_get_bus_handle(0, &_bus_handle));

    i2c_device_config_t _dev_config;
    memset(&_dev_config, 0, sizeof(_dev_config));
    _dev_config.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    _dev_config.device_address = HDC1080_I2C_ADDRESS;
    _dev_config.scl_speed_hz = _freq;
    ERR_LOG_RET(i2c_master_bus_add_device(_bus_handle, &_dev_config, &_dev_handle));
}

/**
 * @brief Class constructor.
 * 
 * @param _sda_pin GPIO number of sda pin.
 * @param _scl_pin GPIO numbero of scl pin.
 * @param _freq I2C communication frequency
 * @param _i2c_port_num Internal I2C peripheral number.
 * @param _timeout_ms BUS transmission timeout time in milliseconds.
 * @param _init_bus Initialize the I2C peripheral with the provided parameters.
 * 
 * @returns A new HDC1080 istance.
 */
HDC1080::HDC1080(gpio_num_t _sda_pin, gpio_num_t _scl_pin, uint32_t _freq, i2c_port_t _i2c_port_num, uint32_t _timeout_ms, bool _init_bus)
{
    //Store parameter for later use
    _tout_ms = _timeout_ms;
    
    //Setup I2C Communication
    memset(&_bus_handle, 0, sizeof(_bus_handle));
    memset(&_dev_handle, 0, sizeof(_dev_handle));

    if(_init_bus)
    {
        i2c_master_bus_config_t _bus_config;
        memset(&_bus_config, 0, sizeof(_bus_config));
        _bus_config.i2c_port =  _i2c_port_num;
        _bus_config.sda_io_num = _sda_pin;
        _bus_config.scl_io_num = _scl_pin;
        _bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
        _bus_config.glitch_ignore_cnt = 7;
        _bus_config.intr_priority = 0;
        _bus_config.flags.enable_internal_pullup = 1;
        _bus_config.flags.allow_pd = 0;
        ERR_LOG_RET(i2c_new_master_bus(&_bus_config, &_bus_handle));
    }
    else ERR_LOG_RET(i2c_master_get_bus_handle(0, &_bus_handle));

    i2c_device_config_t _dev_config;
    memset(&_dev_config, 0, sizeof(_dev_config));
    _dev_config.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    _dev_config.device_address = HDC1080_I2C_ADDRESS;
    _dev_config.scl_speed_hz = _freq;
    ERR_LOG_RET(i2c_master_bus_add_device(_bus_handle, &_dev_config, &_dev_handle));
}

/**
 * @brief Configure HDC1080 with default configuration.
 * 
 * @returns ESP_OK if successful.
 */
esp_err_t HDC1080::configureDefault()
{
    hdc1080_config_t _config;
    memset(&_config, 0, sizeof(_config));
    _config.flags.heater = HDC1080_HEATER_DISABLED;
    _config.flags.mode_of_acquisition = HDC1080_ACQUISITION_HUMIDITY_AND_TEMPERATURE;
    _config.flags.battery_status = HDC1080_BATTERY_STATUS_LOW;
    _config.flags.temperature_measurement_resolution = HDC1080_TEMPERATURE_RESOLUTION_14BIT;
    _config.flags.humidity_measurement_resolution = HDC1080_HUMIDITY_RESOLUTION_14BIT;

    return configure(_config);
}

/**
 * @brief Configure HDC1080 with user provided configuration.
 * 
 * @param _config HDC1080 configuration structure.
 * 
 * @returns ESP_OK if successful.
 */
esp_err_t HDC1080::configure(hdc1080_config_t _config)
{
    //check if the correct device is addressed
    ERR_LOG_RET_ERR(_checkDevManID());
    
    ERR_LOG_RET_ERR(_writeReg(HDC1080_CONFIG_REG, &_config.hdc1080_config_value, sizeof(_config.hdc1080_config_value)));
    ERR_LOG_RET_ERR(measure());

    return ESP_OK;
}

/**
 * @brief Start a measurement on the device.
 * 
 * @returns ESP_OK on success.
 */
esp_err_t HDC1080::measure()
{
    if((esp_timer_get_time() - _latest_measure_time) >= HDC1080_MIN_SAMPLE_TIME_MS*1000)
    {
        uint8_t _temp_raw[4];
        ERR_LOG(_readReg(_temp_raw, sizeof(_temp_raw)));
    
        uint16_t _temp_sample = ((uint16_t)_temp_raw[0]<<8)+_temp_raw[1];
        uint16_t _hum_sample = ((uint16_t)_temp_raw[2]<<8)+_temp_raw[3];

        _latest_temp = (float)_temp_sample*165/65536 - 40;
        _latest_hum = (float)_hum_sample*100/65536;

        ERR_LOG_RET_ERR(_writeReg(HDC1080_TEMPERATURE_REG, NULL, 0));
        return ESP_OK;
        _latest_measure_time = esp_timer_get_time();
    }
    else return HDC1080_CONVERTING;
}

/**
 * @brief Get temperature reading.
 * 
 * @returns The measured temperature in Celsius;
 */
float HDC1080::getTemp()
{
    return _latest_temp;
}

/**
 * @brief Get relative humidity reading.
 * 
 * @returns The measured humidity percentage;
 */
float HDC1080::getHum()
{
    return _latest_hum;
}
