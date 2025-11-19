#pragma once
/* ------------------------- HEADER (.h) FILE INFORMATION ------------------------- */
/**
 * PROJECT:         ENV.EYE SPOT Project
 * 
 * FILE:            "nano_esp32.hpp"
 * DESCRIPTION:     Arduino Nano Esp32 board configuration component. 
 *                  Provides a fast way to configure the two main hardware peripherals 
 *                  available onboard: LED_BUILTIN (PIN 13) and RGB_LED (GPIO_0, GPIO_45, GPIO_46)
 * 
 */

#include "driver/gpio.h"
#include "esp_err.h"

namespace Nano_ESP32
{    
    const gpio_num_t LED_BUILTIN = GPIO_NUM_48;
    const gpio_num_t RGB_RED = GPIO_NUM_46;
    const gpio_num_t RGB_BLUE = GPIO_NUM_45;
    const gpio_num_t RGB_GREEN = GPIO_NUM_0;
    
    /**
     * @brief Setup onboard peripherals.
     * 
     * @returns ESP_OK if successful. 
     */
    esp_err_t configure_nano();

        
    /**
     * @brief Change duty cycle on RGB LED pins.
     * 
     * @param _gpio RGB LED GPIO Number.
     * @param _duty Duty Cycle to output on the specified GPIO.
     * 
     * @returns void
     */
    void set_rgb_pwm(gpio_num_t _gpio, uint8_t _duty);

    /**
     * @brief Set LED_BUILTIN output value.
     * 
     * @param _value Output value.
     * @returns void
     */
    void set_led_builtin(bool _value);
}

