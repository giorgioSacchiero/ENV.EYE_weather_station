#pragma once
/* ------------------------- HEADER (.h) FILE INFORMATION ------------------------- */
/**
 * PROJECT:         ENV.EYE SPOT Project
 * 
 * FILE:            "wf183de_registers.h"
 * DESCRIPTION:     WF183DE ambient pressure and temperature sensor API.
 * 
 */

#include "inttypes.h"
#include "driver/i2c_master.h"

class WF183DE
{
   public:
   WF183DE(gpio_num_t _sda_pin, gpio_num_t _scl_pin, uint32_t _freq, i2c_port_t _i2c_port_num, bool _init_bus);
   WF183DE(gpio_num_t _sda_pin, gpio_num_t _scl_pin, uint32_t _freq, i2c_port_t _i2c_port_num, uint32_t _timeout_ms, bool _init_bus);
   
   esp_err_t measureTemp();
   esp_err_t measurePressure();
   float getTemp();
   float getPressure();

   private:
   esp_err_t _writeReg(uint8_t _reg_num, void *_write_data, uint8_t _size);
   esp_err_t _writeReadReg(uint8_t _reg_num, void *_read_data, uint8_t _size);
   esp_err_t _readReg(void *_read_data, uint8_t _size);
   
   i2c_master_bus_handle_t _bus_handle;
   i2c_master_dev_handle_t _dev_handle;
   uint32_t _tout_ms = 10;
   
   uint64_t _last_temp_measure_time = 0, _last_pressure_measure_time = 0;
   float _latest_temp = -1, _latest_pressure = -1; 
};