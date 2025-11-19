#pragma once
/* ------------------------- HEADER (.h) FILE INFORMATION ------------------------- */
/**
 * PROJECT:         ENV.EYE HUB Project
 * 
 * FILE:            "led_mgmt.h"
 * DESCRIPTION:     Onboard RGB addressable LEDs (WS2812) management.
 * 
*/

#include "Bonezegei_WS2812.h"

#define RGB_PIN 6
#define RGB_LED_NUMBER 3
#define RGB_MAX_BRIGHTNESS 0x0A

void led_begin();
void led_fade(uint8_t red, uint8_t green, uint8_t blue, uint8_t increment, bool end_pause, TickType_t pause_ticks);
bool led_fade_end();
void led_fade_reset();