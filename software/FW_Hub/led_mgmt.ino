/* ------------------------- C/C++ (.ino) FILE INFORMATION ------------------------- */
/**
 * PROJECT:         ENV.EYE HUB Project
 * 
 * FILE:            "led_mgmt.ino"
 * 
 */

#include "led_mgmt.h"

static bool fade_en = false, fading_up = true, fade_pause_en = false;
static uint8_t fade_blue = 0, fade_green = 0, fade_red = 0;
static uint8_t blue_fade_en = 0, green_fade_en = 0, red_fade_en = 0;
static uint8_t fade_increment = 1;
static TickType_t fade_pause_ticks = 0; 
static bool end_flag = false;

static Bonezegei_WS2812 rgb(RGB_PIN, RGB_LED_NUMBER);

static void rgb_fade(uint8_t red, uint8_t green, uint8_t blue)
{
  if(fading_up)
  {
    if(fade_green == (RGB_MAX_BRIGHTNESS - fade_increment)) fading_up = !fading_up;
    fade_red += fade_increment; fade_green += fade_increment; fade_blue += fade_increment;
    uint32_t fade_data = ((uint32_t)fade_red*red<<16) + ((uint32_t)fade_green*green<<8) + fade_blue*blue;
    rgb.fill(fade_data);
  }
  else
  {
    if(fade_green == fade_increment)
    {
      fading_up = !fading_up;
      end_flag = true;
    } 
    fade_red -= fade_increment; fade_green -= fade_increment; fade_blue -= fade_increment;
    uint32_t fade_data = ((uint32_t)fade_red*red<<16) + ((uint32_t)fade_green*green<<8) + fade_blue*blue;
    rgb.fill(fade_data);
  }
}

static bool rgb_fade_end()
{
  return fade_red == 0x00;
}

void rgb_task(void* _args)
{
  while(1)
  {
    if(fade_en)
    {
      rgb_fade(red_fade_en, green_fade_en, blue_fade_en);

      if(fade_pause_en && rgb_fade_end())
        vTaskDelay(fade_pause_ticks);
      else vTaskDelay(75);
      continue;
    }
    vTaskDelay(100);
  }
}

void led_begin()
{
  rgb.begin();
  xTaskCreatePinnedToCore(rgb_task, "rgb_task", 8192, NULL, 1, NULL, 1);
}

void led_fade(uint8_t red, uint8_t green, uint8_t blue, uint8_t increment, bool end_pause, TickType_t pause_ticks)
{
  fade_en = (red == 0 && green == 0 && blue == 0) ? false : true;
  if(fade_en) end_flag = false; //avoid glitches when setting a new fade mode and the sequence is ended
  
  red_fade_en = red;
  green_fade_en = green;
  blue_fade_en = blue;
  fade_increment = increment;
  
  if(fade_pause_en = end_pause)
  {
    fade_pause_ticks = pause_ticks;
  }
}

void led_fade_reset()
{
  fade_blue = 0; fade_green = 0; fade_red = 0;
  fading_up = true;
}

bool led_fade_end()
{
  return end_flag;
}