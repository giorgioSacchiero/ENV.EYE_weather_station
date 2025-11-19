/* ------------------------- C++ (.cpp) FILE INFORMATION ------------------------- */
/**
 * PROJECT:         ENV.EYE SPOT Project
 * 
 * FILE:            "env_eye_main.cpp"
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_sleep.h"
#include "esp_timer.h"

#include "nano_esp32.hpp"
#include "hdc1080.hpp"
#include "wf183de.hpp"
#include "syp16.hpp"
#include "lora.h"

#include "env_eye_pinout.h"
#include "env_eye_config.h"
#include "env_eye_lora.hpp"

/* ------------------------- MACRO DEFINITION -------------------------*/
#define TAG "SPOT"

#define ERR_LOG_RET_ERR(FUNCT)  \
if ( esp_err_t _ERR = FUNCT != ESP_OK ) \
{   \
    ESP_LOGE(TAG, "ERROR (%X) at line %d. Description: %s", _ERR, __LINE__, esp_err_to_name(_ERR)); \
    return _ERR; \
}

/* ------------------------- NAMESPACES ------------------------- */
using namespace Nano_ESP32;

/* ------------------------- GLOBAL VARIABLES ------------------------- */
adc_oneshot_unit_handle_t adc_unit_handle;
RTC_DATA_ATTR int wakeup_count = 0;
enum direction_t {N, NE, E, SE, S, SW, W, NW, UNK};
uint16_t geiger_pulses = 0;

/* ------------------------- LOCAL FUNCTIONS ------------------------- */
esp_err_t gpio_configure();

inline void sensor_5v_turn_on();
inline void sensor_5v_turn_off();

inline void sensor_3v3_turn_on();
inline void sensor_3v3_turn_off();

inline void sensor_25v_turn_on();
inline void sensor_25v_turn_off();

inline void geiger_25v_turn_on();
inline void geiger_25v_turn_off();

inline void geiger_5v_turn_on();
inline void geiger_5v_turn_off();

inline void geiger_turn_on();
inline void geiger_turn_off();

float get_wind_speed();
direction_t get_wind_dir();

float get_noise_level();

float get_pollution_level(SYP16 _syp);

void geiger_isr(void *_args);
float get_radiation_level();

void lora_send_packet(lora_payload_t *_payload);

void esp_enter_sleep();

extern "C" void app_main(void)
{
    //Wait some time for serial monitor connection
    vTaskDelay(100);

    //Configure GPIO
    ESP_ERROR_CHECK(configure_nano());
    ESP_ERROR_CHECK(gpio_configure());

    //Turn on 3.3V sensor rail and initialize communicatin
    sensor_3v3_turn_on();
    vTaskDelay(100);
    
    HDC1080 hdc(rev1_pinout.I2C_SDA, rev1_pinout.I2C_SCL, 100000, I2C_NUM_0, 1000, true);
    hdc.configureDefault();
    WF183DE wf(rev1_pinout.I2C_SDA, rev1_pinout.I2C_SCL, 100000, I2C_NUM_0, 1000, false);
    
    //Configure LoRa module
    lora_configure(100000, 868000000, 7, 7, 1, true);

    vTaskDelay(100);
    ESP_LOGE(TAG, "Wakeup num %d", wakeup_count);

    while(1)
    {
        lora_payload_t payload;
        lora_init_payload(&payload);

        switch(wakeup_count)
        {
            //----> FIRST START, wait some time before measuring.
            case 0:
                wakeup_count++;
                esp_enter_sleep();
                break;
            
            //----> MEASURE ONLY TEMP., HUM. AND PRESSURE
            case 1:     
                hdc.measure();

                //Perform two measures to avoid null reading
                wf.measurePressure();
                vTaskDelay(10);
                wf.measurePressure();

                payload.temperature = hdc.getTemp();
                payload.humidity = hdc.getHum();
                payload.air_pressure = wf.getPressure();
                
                lora_send_packet(&payload);
                //wait for Lora module to send the data before shutting off
                vTaskDelay(100);
                sensor_3v3_turn_off();
                
                wakeup_count++;
                esp_enter_sleep();
            break;
            
            //----> MEASURE TEMP., HUM., PRESSURE AND WIND
            case 2:
                hdc.measure();
                
                //Perform two measures to avoid null reading
                wf.measurePressure();
                vTaskDelay(10);
                wf.measurePressure();

                payload.temperature = hdc.getTemp();
                payload.humidity = hdc.getHum();
                payload.air_pressure = wf.getPressure();
                
                if(gpio_get_level(rev1_pinout.CAP_VOLTAGE_OK))
                {
                    sensor_25v_turn_on();
                    vTaskDelay(150);
                    payload.wind_spd = get_wind_speed();
                    payload.wind_dir = (int16_t)get_wind_dir();
                    sensor_25v_turn_off();
                
                    sensor_5v_turn_on();
                    vTaskDelay(300);
                    payload.noise_level = get_noise_level();
                    sensor_5v_turn_off();

                    wakeup_count++;
                }
                else wakeup_count = 1;
                
                lora_send_packet(&payload);
                vTaskDelay(100);
                //wait for Lora module to send the data before shutting off
                sensor_3v3_turn_off();                

                esp_enter_sleep();
            break;

            //----> MEASURE TEMP., HUM., PRESSURE, BACKGR. NOISE AND RADIATION 
            case 3: //temp + hum + press + mic + geig
                hdc.measure();
                
                //Perform two measures to avoid null reading
                wf.measurePressure();
                vTaskDelay(10);
                wf.measurePressure();

                payload.temperature = hdc.getTemp();
                payload.humidity = hdc.getHum();
                payload.air_pressure = wf.getPressure();
 
                if(gpio_get_level(rev1_pinout.CAP_VOLTAGE_OK))
                {               
                    sensor_5v_turn_on();
                    vTaskDelay(300);
                    payload.noise_level = get_noise_level();
                    sensor_5v_turn_off();

                    geiger_turn_on();
                    vTaskDelay(50);
                    float usievert_h = get_radiation_level();
                    if(usievert_h != -1) 
                    {
                        payload.radiation_level = usievert_h;
                        wakeup_count++;
                    }
                    geiger_turn_off();
                }
                
                lora_send_packet(&payload);
                vTaskDelay(100);
                //wait for Lora module to send the data before shutting off
                sensor_3v3_turn_off();                
                esp_enter_sleep();
            break;

            //----> MEASURE TEMP., HUM., PRESSURE, BACKGR. NOISE AND POLLUTION 
            case 4: //temp + hum + press + wind + mic + air
                hdc.measure();
                
                //Perform two measures to avoid null reading
                wf.measurePressure();
                vTaskDelay(10);
                wf.measurePressure();

                payload.temperature = hdc.getTemp();
                payload.humidity = hdc.getHum();
                payload.air_pressure = wf.getPressure();

                if(gpio_get_level(rev1_pinout.CAP_VOLTAGE_OK))
                {
                    sensor_5v_turn_on();
                    vTaskDelay(150);
                    SYP16 syp(rev1_pinout.UART_TX, rev1_pinout.UART_RX, UART_NUM_1, SYP16_QUESTION_ANSWER_MODE);
                    vTaskDelay(150);
                    payload.noise_level = get_noise_level();
                
                    payload.air_pollution_level = get_pollution_level(syp);
                    wakeup_count = 2; //Start again from measuring wind 
                }
                //When supercapacitors have low charge, measure with geiger first, since it draws less power.
                else wakeup_count = 3;
                sensor_5v_turn_off();

                lora_send_packet(&payload);
                vTaskDelay(100);
                sensor_3v3_turn_off();
                
                esp_enter_sleep();
            break;
        }

        vTaskDelay(100); //Never reached in normal operation
    }
}

/* ------------------------- GPIO Management ------------------------- */
esp_err_t gpio_configure()
{
    //Initialize digital inputs
    gpio_config_t _pin_config;
    _pin_config.mode = GPIO_MODE_INPUT;
    _pin_config.intr_type = GPIO_INTR_DISABLE;
    _pin_config.pin_bit_mask = 1ULL << rev1_pinout.CAP_VOLTAGE_OK | 1ULL<<rev1_pinout.GEIGER_IN;
    _pin_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    _pin_config.pull_up_en = GPIO_PULLUP_ENABLE;
    ERR_LOG_RET_ERR(gpio_config(&_pin_config));

    //Geiger input interrupt
    gpio_set_intr_type(rev1_pinout.GEIGER_IN, GPIO_INTR_NEGEDGE);
    gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
    gpio_isr_handler_add(rev1_pinout.GEIGER_IN, geiger_isr, NULL);
    gpio_intr_disable(rev1_pinout.GEIGER_IN);

    //Initialize digital outputs
    _pin_config.mode = GPIO_MODE_OUTPUT;
    _pin_config.pin_bit_mask = 1ULL<<rev1_pinout.EN_3V3_RAIL;
    ERR_LOG_RET_ERR(gpio_config(&_pin_config));
    ERR_LOG_RET_ERR(gpio_set_level(rev1_pinout.EN_3V3_RAIL, 1));

    _pin_config.pin_bit_mask = 1ULL<<rev1_pinout.EN_5V_SENSOR_RAIL | 1ULL<<rev1_pinout.EN_5V_GEIGER_RAIL | 1ULL<<rev1_pinout.EN_25V_RAIL | 1ULL<<rev1_pinout.EN_SERVO_RAIL;
    _pin_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    _pin_config.pull_up_en = GPIO_PULLUP_DISABLE;
    ERR_LOG_RET_ERR(gpio_config(&_pin_config));
    ERR_LOG_RET_ERR(gpio_set_level(rev1_pinout.EN_5V_SENSOR_RAIL, 0));
    ERR_LOG_RET_ERR(gpio_set_level(rev1_pinout.EN_5V_GEIGER_RAIL, 0));
    ERR_LOG_RET_ERR(gpio_set_level(rev1_pinout.EN_25V_RAIL, 0));
    ERR_LOG_RET_ERR(gpio_set_level(rev1_pinout.EN_SERVO_RAIL, 0));
    
    //Initialize analog inputs
    adc_oneshot_unit_init_cfg_t _unit_config;
    _unit_config.unit_id = ADC_UNIT_1;
    _unit_config.ulp_mode = ADC_ULP_MODE_DISABLE;
    ERR_LOG_RET_ERR(adc_oneshot_new_unit(&_unit_config, &adc_unit_handle));

    adc_oneshot_chan_cfg_t _channel_config;
    _channel_config.bitwidth = ADC_BITWIDTH_DEFAULT;
    _channel_config.atten = ADC_ATTEN_DB_12;
    ERR_LOG_RET_ERR(adc_oneshot_config_channel(adc_unit_handle, rev1_pinout.MIC_IN, &_channel_config));
    ERR_LOG_RET_ERR(adc_oneshot_config_channel(adc_unit_handle, rev1_pinout.WIND_SPEED_IN, &_channel_config));
    ERR_LOG_RET_ERR(adc_oneshot_config_channel(adc_unit_handle, rev1_pinout.WIND_DIR_IN, &_channel_config));

    return ESP_OK;
}

// -------- Toggle 3V3 rail -------- 
inline void sensor_3v3_turn_on()
{
    gpio_set_level(rev1_pinout.EN_3V3_RAIL, 0);
}

inline void sensor_3v3_turn_off()
{
    gpio_set_level(rev1_pinout.EN_3V3_RAIL, 1);
}

// -------- Toggle 5V sensor rail -------- 
inline void sensor_5v_turn_on()
{
    gpio_set_level(rev1_pinout.EN_5V_SENSOR_RAIL, 1);
}

inline void sensor_5v_turn_off()
{
    gpio_set_level(rev1_pinout.EN_5V_SENSOR_RAIL, 0);
}

// -------- Toggle 5V geiger rail --------
inline void geiger_5v_turn_on()
{
    gpio_set_level(rev1_pinout.EN_5V_GEIGER_RAIL, 1);
}

inline void geiger_5v_turn_off()
{
    gpio_set_level(rev1_pinout.EN_5V_GEIGER_RAIL, 0);
}

// -------- Toggle 25V rail -------- 
inline void sensor_25v_turn_on()
{
    gpio_set_level(rev1_pinout.EN_SERVO_RAIL, 1);
}

inline void sensor_25v_turn_off()
{
    gpio_set_level(rev1_pinout.EN_SERVO_RAIL, 0);
}

// -------- Toggle 25V rail for geiger supply -------- 
inline void geiger_25v_turn_on()
{
    gpio_set_level(rev1_pinout.EN_25V_RAIL, 1);
}

inline void geiger_25v_turn_off()
{
    gpio_set_level(rev1_pinout.EN_25V_RAIL, 0);
}

// -------- Toggle geiger supply --------
inline void geiger_turn_on()
{
    geiger_25v_turn_on();
    geiger_5v_turn_on();
}

inline void geiger_turn_off()
{
    geiger_25v_turn_on();
    geiger_5v_turn_off();
}

//--------Wind Measurements--------
float get_wind_speed()
{
    int _raw_wind;
    if(adc_oneshot_read(adc_unit_handle, rev1_pinout.WIND_SPEED_IN, &_raw_wind) != ESP_OK)
    {
        return -99; //sost con latest_wind_spd
    }
    return (60*_raw_wind/4095.0);
}

direction_t get_wind_dir()
{
    int _raw_direction;
    direction_t _dir;
    if(adc_oneshot_read(adc_unit_handle, rev1_pinout.WIND_DIR_IN, &_raw_direction) != ESP_OK)
        return UNK;

    for(int i = 0; i < 8; i++)
    {   
        if(abs((_raw_direction - WIND_DIR_CALIBRATION[i])) > WIND_DIR_STEP_DELTA)
            continue;
        else _dir = (direction_t)i;
        vTaskDelay(1);
    }
    //adjust for mounting direction
    return (direction_t)(((int)_dir + (int)MOUNT_DIRECTION) % 8);  
}

//--------Noise Measurement--------
float get_noise_level()
{
    int _raw_noise;
    uint64_t mean_noise = 0;
    for(int i=0;i<MIC_SAMPLE_NUM;i++)
    {
        ESP_ERROR_CHECK_WITHOUT_ABORT(adc_oneshot_read(adc_unit_handle, rev1_pinout.MIC_IN, &_raw_noise));
        mean_noise += abs(_raw_noise - MIC_DC_OFFSET);
        vTaskDelay(1);
    }
    return 10*log(mean_noise*2/MIC_SAMPLE_NUM) + MIC_DB_OFFSET;
}

//--------Pollution Measurement--------
float get_pollution_level(SYP16 _syp)
{
    uint64_t _start_time = esp_timer_get_time();
    while((esp_timer_get_time() - _start_time) < 180000000L)
    {
        vTaskDelay(1);
        if(!gpio_get_level(rev1_pinout.CAP_VOLTAGE_OK))
            return -99;
    }
    ESP_ERROR_CHECK(_syp.measure());
    while(!_syp.dataAvailable()) vTaskDelay(1);
    return _syp.getGasConcentration();
}

//--------Radiation Measurement--------
void geiger_isr(void *_args)
{
    if(geiger_pulses < 0xFFFF) geiger_pulses++;
}

float get_radiation_level()
{
    //Enable counting
    gpio_intr_enable(rev1_pinout.GEIGER_IN);

    //Wait 15s to get the measurements
    uint64_t _start_time = esp_timer_get_time();
    while(((esp_timer_get_time() - _start_time) <= 15000000) && gpio_get_level(rev1_pinout.CAP_VOLTAGE_OK))
    {   
        vTaskDelay(1);
        if(!gpio_get_level(rev1_pinout.CAP_VOLTAGE_OK))
        {
            gpio_intr_disable(rev1_pinout.GEIGER_IN);
            return -1;
        } 
    }
    gpio_intr_disable(rev1_pinout.GEIGER_IN);
    return (float)geiger_pulses * GEIGER_USV_HOUR_PER_PULSE_S / 15.0;
}

//--------LoRa send packet--------
void lora_send_packet(lora_payload_t *_payload) //finire payload
{   
    lora_packet_t _to_send;
    _to_send.data.module_id = SPOT_MODULE_ID;
    _to_send.data.wakeup_num = wakeup_count;
    _to_send.data.temperature = ( (_payload->temperature != -99) ? round<int16_t>(_payload->temperature*100) : -99 );
    _to_send.data.humidity = ( (_payload->humidity != -99) ? round<int16_t>(_payload->humidity*100) : -99 );
    _to_send.data.air_pressure = ( (_payload->air_pressure != -99) ? round<int32_t>(_payload->air_pressure) : -99 );
    _to_send.data.air_pollution_level = ( (_payload->air_pollution_level != -99) ? round<int16_t>(_payload->air_pollution_level*100) : -99 );
    _to_send.data.wind_spd = ( (_payload->wind_spd != -99) ? round<int16_t>(_payload->wind_spd*100) : -99 );
    _to_send.data.wind_dir = ( (_payload->wind_dir != -99) ? (int16_t)_payload->wind_dir*100 : -99 );
    _to_send.data.noise_level = ( (_payload->noise_level != -99) ? round<int16_t>(_payload->noise_level*100) : -99 );
    _to_send.data.radiation_level = ( (_payload->radiation_level != -99) ? round<int16_t>(_payload->radiation_level*100) : -99 );
    ESP_ERROR_CHECK_WITHOUT_ABORT(lora_send_measurement_data(&_to_send)); //return
}

void esp_enter_sleep()
{
    esp_sleep_enable_timer_wakeup(SLEEP_TIME_US);
    esp_deep_sleep_start();
}