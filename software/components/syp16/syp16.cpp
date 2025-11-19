/* ------------------------- C++ (.cpp) FILE INFORMATION ------------------------- */
/**
 * PROJECT:         ENV.EYE SPOT Project
 * 
 * FILE:            "syp16.cpp"
 * 
 */

#include "math.h"

#include "esp_log.h"
#include "syp16.hpp"
#include "syp16_registers.h"
#include "esp_timer.h"


#define TAG "SYP16"
#define UART_QUEUE_SIZE 10
#define SYP16_MIN_SAMPLE_TIME_MS 500

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

esp_err_t SYP16::_uart_send(const uint8_t *_data, uint32_t _size)
{
    int16_t _ret = uart_write_bytes(_port_num, _data, _size);
    
    if(_ret == -1) return ESP_FAIL;
    else if(_ret != _size) return ESP_ERR_NO_MEM;
    
    return ESP_OK;
}

/**
 * @brief Creates an instance of SYP16
 * 
 * @param _tx_pin GPIO number of tx pin.
 * @param _rx_pin GPIO numbero of rx pin.
 * @param _uart_port_num UART port number
 * @param _comm_mode Sets ACTIVE UPLOAD MODE or Q&A MODE for data request
 * 
 * @returns A new SYP16 istance.
 */
SYP16::SYP16(gpio_num_t _tx_pin, gpio_num_t _rx_pin, uart_port_t _uart_port_num, syp16_mode_config_t _comm_mode)
{
    //Store port number
    _port_num = _uart_port_num;

    //install uart driver
    ERR_LOG_RET(uart_driver_install(_uart_port_num, _uart_buf_size, _uart_buf_size, UART_QUEUE_SIZE, &_uart_queue, 0));

    //configure peripheral
    uart_config_t _uart_conf;
    memset(&_uart_conf, 0, sizeof(_uart_conf));
    _uart_conf.baud_rate = 9600;
    _uart_conf.data_bits = UART_DATA_8_BITS;
    _uart_conf.parity = UART_PARITY_DISABLE;
    _uart_conf.stop_bits = UART_STOP_BITS_1;
    _uart_conf.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;

    ERR_LOG_RET(uart_param_config(_uart_port_num, &_uart_conf));
    ERR_LOG_RET(uart_set_mode(_port_num, UART_MODE_UART));
    //ERR_LOG_RET(uart_set_line_inverse(_port_num, UART_SIGNAL_IRDA_TX_INV | UART_SIGNAL_IRDA_RX_INV));

    //set communication pins
    ERR_LOG_RET(uart_set_pin(_uart_port_num, _tx_pin, _rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    //set communication mode
    if(_comm_mode == SYP16_ACTIVE_UPLOAD_MODE)
    {
        ERR_LOG_RET(_uart_send(SET_ACTIVE_UPLOAD_MODE, sizeof(SET_ACTIVE_UPLOAD_MODE)));
    }
    else if(_comm_mode == SYP16_QUESTION_ANSWER_MODE)
    {   
        ERR_LOG_RET(_uart_send(SET_QUESTION_ANSWER_MODE, sizeof(SET_QUESTION_ANSWER_MODE)));
    }
}  

bool SYP16::dataAvailable()
{
    size_t _buff_size;
    ERR_LOG(uart_get_buffered_data_len(_port_num, &_buff_size));
    return _buff_size != 0;
}

esp_err_t SYP16::measure()
{
    if((esp_timer_get_time() - _air_last_measure_time) >= SYP16_MIN_SAMPLE_TIME_MS*1000)
    {
        //air data request
        ERR_LOG_RET_ERR(_uart_send(GET_GAS_CONCENTRATION, sizeof(GET_GAS_CONCENTRATION)));
        
        _air_last_measure_time = esp_timer_get_time();
        return ESP_OK;
    }
    return SYP16_CONVERTING;
}

float SYP16::getGasConcentration()
{
    // Read data from UART.
    ERR_LOG(uart_get_buffered_data_len(_port_num, (size_t*)&_air_data_length));

    if(_air_data_length == 9)
    {
        uint8_t _air_raw[9];
        uart_read_bytes(_port_num, _air_raw, _air_data_length, 100);

        ERR_LOG(uart_flush(_port_num));

        //checksum calculation
        uint16_t _checksum = 0;
        for(int i = 1; i < 8; i++)
            _checksum += _air_raw[i];

        if((uint8_t)(~_checksum + 1) == _air_raw[8])
        {
            uint16_t _air_sample = ((uint16_t)_air_raw[2]<<8) + _air_raw[3];
            _latest_air = (float)_air_sample*pow(10, (-1)*_air_raw[4]);
        }
    }
    else if(_air_data_length > 0) ESP_LOGE(TAG, "Error while reading data from SYP16. Expected 9 bytes, received %d.", _air_data_length);

    return _latest_air;
}
