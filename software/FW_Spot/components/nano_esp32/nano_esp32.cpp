/* ------------------------- C++ (.cpp) FILE INFORMATION ------------------------- */
/**
 * PROJECT:         ENV.EYE SPOT Project
 * 
 * FILE:            "nano_esp32.cpp"
 * 
 */

#include "nano_esp32.hpp"
#include "esp_log.h"
#include "driver/ledc.h"

namespace Nano_ESP32
{
    static ledc_channel_t RGB_RED_PWM_CHANNEL = LEDC_CHANNEL_0;
    static ledc_channel_t RGB_GREEN_PWM_CHANNEL = LEDC_CHANNEL_1;
    static ledc_channel_t RGB_BLUE_PWM_CHANNEL = LEDC_CHANNEL_2;

    esp_err_t configure_nano()
    {
        //LED_BUILTIN GPIO configuration
        gpio_config_t _led_builtin_conf;
        _led_builtin_conf.mode = GPIO_MODE_OUTPUT;
        _led_builtin_conf.intr_type = GPIO_INTR_DISABLE;
        _led_builtin_conf.pin_bit_mask = 1ULL<<LED_BUILTIN;
        _led_builtin_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
        _led_builtin_conf.pull_up_en = GPIO_PULLUP_ENABLE;

        if(esp_err_t _ret = gpio_config(&_led_builtin_conf) != ESP_OK)
            return _ret;
        
        
        //RGB led PWM configuration
        ledc_timer_config_t _rgb_pwm_timer_conf;
        _rgb_pwm_timer_conf.speed_mode = LEDC_LOW_SPEED_MODE;
        _rgb_pwm_timer_conf.duty_resolution = LEDC_TIMER_4_BIT;
        _rgb_pwm_timer_conf.timer_num = LEDC_TIMER_0;
        _rgb_pwm_timer_conf.freq_hz = 4000;
        _rgb_pwm_timer_conf.clk_cfg = LEDC_AUTO_CLK;
        _rgb_pwm_timer_conf.deconfigure = false;
        
        if(esp_err_t _ret = ledc_timer_config(&_rgb_pwm_timer_conf) != ESP_OK)
            return _ret;

        ledc_channel_config_t _rgb_pwm_channel_conf;
        _rgb_pwm_channel_conf.gpio_num = RGB_RED;
        _rgb_pwm_channel_conf.speed_mode = LEDC_LOW_SPEED_MODE;
        _rgb_pwm_channel_conf.channel = RGB_RED_PWM_CHANNEL,
        _rgb_pwm_channel_conf.intr_type = LEDC_INTR_DISABLE;
        _rgb_pwm_channel_conf.timer_sel = LEDC_TIMER_0;
        _rgb_pwm_channel_conf.duty = 0;
        _rgb_pwm_channel_conf.hpoint = 0;
        _rgb_pwm_channel_conf.sleep_mode = LEDC_SLEEP_MODE_NO_ALIVE_NO_PD;
        _rgb_pwm_channel_conf.flags.output_invert = 1;

        
        if(esp_err_t _ret = ledc_channel_config(&_rgb_pwm_channel_conf) != ESP_OK)
            return _ret;

        _rgb_pwm_channel_conf.gpio_num = RGB_GREEN;
        _rgb_pwm_channel_conf.channel = RGB_GREEN_PWM_CHANNEL;
        
        if(esp_err_t _ret = ledc_channel_config(&_rgb_pwm_channel_conf) != ESP_OK)
            return _ret;

        _rgb_pwm_channel_conf.gpio_num = RGB_BLUE;
        _rgb_pwm_channel_conf.channel = RGB_BLUE_PWM_CHANNEL;
        
        if(esp_err_t _ret = ledc_channel_config(&_rgb_pwm_channel_conf) != ESP_OK)
            return _ret;

        return ESP_OK;
    }

    void set_rgb_pwm(gpio_num_t _gpio, uint8_t _duty)
    {
        switch (_gpio)
        {
        case RGB_RED:
            ledc_set_duty(LEDC_LOW_SPEED_MODE, RGB_RED_PWM_CHANNEL, _duty);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, RGB_RED_PWM_CHANNEL);
            break;
        
        case RGB_GREEN:
            ledc_set_duty(LEDC_LOW_SPEED_MODE, RGB_GREEN_PWM_CHANNEL, _duty);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, RGB_GREEN_PWM_CHANNEL);
            break;

        case RGB_BLUE:
            ledc_set_duty(LEDC_LOW_SPEED_MODE, RGB_BLUE_PWM_CHANNEL, _duty);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, RGB_BLUE_PWM_CHANNEL);
            break;

        default:
            break;
        }
    }
    
    void set_led_builtin(bool _value)
    {
        gpio_set_level(LED_BUILTIN, _value == true ? 1 : 0);
    }
}