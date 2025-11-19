/* ------------------------- C++ (.cpp) FILE INFORMATION ------------------------- */
/**
 * PROJECT:         ENV.EYE SPOT Project
 * 
 * FILE:            "wf183de.cpp"
 * 
 */

#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "memory.h"

#include "wf183de.hpp"
#include "wf183de_registers.h"

#define TAG "WF183DE"
#define WF183DE_MIN_SAMPLE_TIME_MS 500

#define ERR_LOG(FUNCT)  \
if ( esp_err_t _ERR = FUNCT != ESP_OK ) \
    ESP_LOGE(TAG, "ERROR (%X) at line %d. Description: %s", _ERR, __LINE__, esp_err_to_name(_ERR));

#define ERR_LOG_RET(FUNCT)  \
if ( esp_err_t _ERR = FUNCT != ESP_OK ) \
{   \
    ESP_LOGE(TAG, "ERROR (%X) at line %d. Description: %s", _ERR, __LINE__, esp_err_to_name(_ERR)); \
    return; \
}

#define ERR_LOG_RET_ERR(FUNCT)  \
if ( esp_err_t _ERR = FUNCT != ESP_OK ) \
{   \
    ESP_LOGE(TAG, "ERROR (%X) at line %d. Description: %s", _ERR, __LINE__, esp_err_to_name(_ERR)); \
    return _ERR; \
}

esp_err_t WF183DE::_writeReg(uint8_t _reg_num, void *_write_data, uint8_t _size)
{
    uint8_t _write_buf[1+_size];
    _write_buf[0] = _reg_num;

    if(_size)
        memcpy(_write_buf+1, _write_data, _size);
    
    ERR_LOG_RET_ERR(i2c_master_transmit(_dev_handle, _write_buf, sizeof(_write_buf), _tout_ms));
    return ESP_OK;
}

esp_err_t WF183DE::_writeReadReg(uint8_t _reg_num, void *_read_data, uint8_t _size)
{
    uint8_t _write_buf = _reg_num;
    ERR_LOG_RET_ERR(i2c_master_transmit_receive(_dev_handle, &_write_buf, sizeof(_write_buf), (uint8_t*)_read_data, _size, _tout_ms));
    return ESP_OK;
}

esp_err_t WF183DE::_readReg(void *_read_data, uint8_t _size)
{
    ERR_LOG_RET_ERR(i2c_master_receive(_dev_handle, (uint8_t*)_read_data, _size, _tout_ms));
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
 * @returns A new WF183DE istance.
 */
WF183DE::WF183DE(gpio_num_t _sda_pin, gpio_num_t _scl_pin, uint32_t _freq, i2c_port_t _i2c_port_num, bool _init_bus)
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
    else ERR_LOG_RET(i2c_master_get_bus_handle(_i2c_port_num, &_bus_handle));

    i2c_device_config_t _dev_config;
    memset(&_dev_config, 0, sizeof(_dev_config));
    _dev_config.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    _dev_config.device_address = WF183DE_I2C_ADDRESS;
    _dev_config.scl_speed_hz = _freq;
    ERR_LOG_RET(i2c_master_bus_add_device(_bus_handle, &_dev_config, &_dev_handle));
    
    //Measure temperature once to read pressure correctly (Datasheet Note)
    measureTemp();
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
 * @returns A new WF183DE istance.
 */
WF183DE::WF183DE(gpio_num_t _sda_pin, gpio_num_t _scl_pin, uint32_t _freq, i2c_port_t _i2c_port_num, uint32_t _timeout_ms, bool _init_bus)
{
    //Store timeout time
    _tout_ms = _timeout_ms;
    
    //Setup I2C Communication
    memset(&_bus_handle, 0, sizeof(_bus_handle));
    memset(&_dev_handle, 0, sizeof(_dev_handle));

    if(_init_bus)
    {
        i2c_master_bus_config_t _bus_config;
        _bus_config.i2c_port =  _i2c_port_num;
        _bus_config.sda_io_num = _sda_pin;
        _bus_config.scl_io_num = _scl_pin;
        _bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
        _bus_config.glitch_ignore_cnt = 5;
        _bus_config.intr_priority = 0;
        _bus_config.flags.enable_internal_pullup = 1;
        _bus_config.flags.allow_pd = 0;
        ERR_LOG_RET(i2c_new_master_bus(&_bus_config, &_bus_handle));
    }
    else ERR_LOG_RET(i2c_master_get_bus_handle(_i2c_port_num, &_bus_handle));

    i2c_device_config_t _dev_config;
    memset(&_dev_config, 0, sizeof(_dev_config));
    _dev_config.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    _dev_config.device_address = WF183DE_I2C_ADDRESS;
    _dev_config.scl_speed_hz = _freq;
    ERR_LOG_RET(i2c_master_bus_add_device(_bus_handle, &_dev_config, &_dev_handle));

    //Measure temperature once to read pressure correctly (Datasheet Note)
    measureTemp();
}

/**
 * @brief Start a temperature measurement on the device.
 * 
 * @returns ESP_OK on success.
 */
esp_err_t WF183DE::measureTemp()
{
    if((esp_timer_get_time() - _last_temp_measure_time) >= WF183DE_MIN_SAMPLE_TIME_MS*1000)
    {
        uint8_t _write_buf = WF183DE_CMD_READ_TEMPERATURE;
        ERR_LOG_RET_ERR(_writeReg(WF183DE_CMD_REG, &_write_buf, sizeof(_write_buf)));
        
        _last_temp_measure_time = esp_timer_get_time();
        return ESP_OK;
    }
    else return WF183DE_CONVERTING;
}

/**
 * @brief Start a pressure measurement on the device.
 * 
 * @returns ESP_OK on success.
 */
esp_err_t WF183DE::measurePressure()
{
    if((esp_timer_get_time() - _last_pressure_measure_time) >= WF183DE_MIN_SAMPLE_TIME_MS*1000)
    {
        uint8_t _write_buf = WF183DE_CMD_READ_PRESSURE;
        ERR_LOG_RET_ERR(_writeReg(WF183DE_CMD_REG, &_write_buf, sizeof(_write_buf)));
        
        _last_pressure_measure_time = esp_timer_get_time();
        return ESP_OK;
    }
    else return WF183DE_CONVERTING;
}

/**
 * @brief Read temperature from the device.
 * 
 * @returns The measured temperature value in Celsius. 
 */
float WF183DE::getTemp()
{
    //First check measurment status
    uint8_t _status_raw;
    ERR_LOG(_writeReadReg(WF183DE_STATUS_REG, &_status_raw, sizeof(_status_raw)));

    if( (_status_raw & 0x01) == WF183DE_STATUS_READY)
    {
        //Read data from device
        uint8_t _temp_raw[2];
        ERR_LOG(_writeReadReg(WF183DE_TEMP_HI_REG, _temp_raw, sizeof(_temp_raw)));
        
        uint16_t _temp_sample = ((uint16_t)_temp_raw[0]<<8) + _temp_raw[1];
        _latest_temp = (float)_temp_sample/10;
    }
    return _latest_temp;
}

/**
 * @brief Read pressure from the device.
 * 
 * @returns The measured pressure value in hPa. 
 */
float WF183DE::getPressure()
{
    //First check measurment status
    uint8_t _status_raw;
    ERR_LOG(_writeReadReg(WF183DE_STATUS_REG, &_status_raw, sizeof(_status_raw)));

    if( (_status_raw % 0x02) == WF183DE_STATUS_READY)
    {
        //Read data from device
        uint8_t _pressure_raw[4];
        ERR_LOG(_writeReadReg(WF183DE_PRESSURE_4_REG, _pressure_raw, sizeof(_pressure_raw)));
        
        uint32_t _pressure_sample = ((uint32_t)_pressure_raw[0]<<24) + ((uint32_t)_pressure_raw[1]<<16) + ((uint32_t)_pressure_raw[2]<<8) + _pressure_raw[0];
        _latest_pressure = (float)_pressure_sample/100;
    }
    return _latest_pressure;
}