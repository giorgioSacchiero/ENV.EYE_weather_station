/* ------------------------- C++ (.cpp) FILE INFORMATION ------------------------- */
/**
 * PROJECT:         ENV.EYE SPOT Project
 * 
 * FILE:            "syp16.cpp"
 * 
 */

#include "esp_log.h"
#include "syp16.hpp"
#include "syp16_registers.h"

#define TAG "WF183DE"
#define UART_QUEUE_SIZE 10

#define ERR_LOG(FUNCT)  \
if ( esp_err_t _ERR = FUNCT != ESP_OK ) \
    ESP_LOGE("ERROR_LOG", "ERROR (%X) at line %d. Description: %s", _ERR, __LINE__, esp_err_to_name(_ERR));

#define ERR_LOG_RET(FUNCT)  \
if ( esp_err_t _ERR = FUNCT != ESP_OK ) \
{   \
    ESP_LOGE("ERROR_LOG", "ERROR (%X) at line %d. Description: %s", _ERR, __LINE__, esp_err_to_name(_ERR)); \
    return; \
}

#define ERR_LOG_RET_ERR(FUNCT)  \
if ( esp_err_t _ERR = FUNCT != ESP_OK ) \
{   \
    ESP_LOGE("ERROR_LOG", "ERROR (%X) at line %d. Description: %s", _ERR, __LINE__, esp_err_to_name(_ERR)); \
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
 */
SYP16::SYP16(gpio_num_t _tx_pin, gpio_num_t _rx_pin, uart_port_t _uart_port_num, syp16_mode_config_t _comm_mode)
{
    //Store port number
    _port_num = _uart_port_num;

    //install uart driver
    ERR_LOG_RET(uart_driver_install(_uart_port_num, _uart_buf_size, _uart_buf_size, UART_QUEUE_SIZE, &_uart_queue, 0));

    //configure peripheral
    uart_config_t _uart_conf;
    _uart_conf.baud_rate = 9600;
    _uart_conf.data_bits = UART_DATA_8_BITS;
    _uart_conf.parity = UART_PARITY_DISABLE;
    _uart_conf.stop_bits = UART_STOP_BITS_1;
    _uart_conf.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    ERR_LOG_RET(uart_param_config(_uart_port_num, &_uart_conf));

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
