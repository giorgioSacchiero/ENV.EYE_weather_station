#pragma once
/* ------------------------- HEADER (.h) FILE INFORMATION ------------------------- */
/**
 * PROJECT:         ENV.EYE SPOT Project
 * 
 * FILE:            "syp16.hpp"
 * DESCRIPTION:     SYP16 air quality sensor API.
 * 
 */
#include "driver/gpio.h"
#include "driver/uart.h"

typedef enum 
{
    SYP16_QUESTION_ANSWER_MODE,
    SYP16_ACTIVE_UPLOAD_MODE
}syp16_mode_config_t;

class SYP16
{
    public:
    SYP16(gpio_num_t _tx_pin, gpio_num_t _rx_pin, uart_port_t _uart_port_num, syp16_mode_config_t _comm_mode);
    bool dataAvailable();
    esp_err_t measure();
    float getGasConcentration();

    private:
    esp_err_t _uart_send(const uint8_t *_data, uint32_t _size);

    uart_port_t _port_num;
    QueueHandle_t _uart_queue;
    const int16_t _uart_buf_size = 1024*2;

    uint64_t _air_last_measure_time = 0;
    int _air_data_length = 0;
    float _latest_air = -1;
};