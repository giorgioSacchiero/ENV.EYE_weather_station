#include "arduino_secrets.h"
/* ------------------------- C/C++ (.ino) FILE INFORMATION ------------------------- */
/**
 * PROJECT:         ENV.EYE HUB Project
 * 
 * FILE:            "HUB.ino"
 * DESCRIPTION:     Eve.Eye HUB main.
 * 
 */

#include <math.h>

#include "thingProperties.h"
#include "lora_conf.h"
#include "led_mgmt.h"

ArduinoIoTCloudEvent last_event = ArduinoIoTCloudEvent::DISCONNECT;
bool packet_fade_triggered = false;
lora_packet_t _rx_packet;

void callback_sync()
{
  led_fade(0,1,0, 1, true, 5000);
  last_event = ArduinoIoTCloudEvent::SYNC;
}

void callback_connect()
{
  led_fade(0, 0, 1, 1, false, 5000);
  last_event = ArduinoIoTCloudEvent::CONNECT;
}

void callback_disconnect()
{
  led_fade(1, 0, 0, 1, false, 5000);
  last_event = ArduinoIoTCloudEvent::DISCONNECT;
}

float round_to_dp( float in_value, int decimal_place )
{
	float multiplier = powf( 10.0f, decimal_place );
	in_value = roundf( in_value * multiplier ) / multiplier;
	return in_value;
}

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(115200);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(3000);

  //Initialize LoRa Module
  lora_begin();

  // Defined in thingProperties.h
  initProperties();

  //Initialize RGB led management
  led_begin();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  ArduinoCloud.addCallback(ArduinoIoTCloudEvent::CONNECT, callback_connect);
  ArduinoCloud.addCallback(ArduinoIoTCloudEvent::DISCONNECT, callback_disconnect);
  ArduinoCloud.addCallback(ArduinoIoTCloudEvent::SYNC, callback_sync);

  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  //Start fading red
  led_fade(1, 0, 0, 1, false, 500);
}

void loop() 
{
  //Update Cloud
  ArduinoCloud.update();

  //Reset fading mode after packet reception
  if(packet_fade_triggered && led_fade_end())
  {
    led_fade_reset();
    packet_fade_triggered = false;
    switch(last_event)
    {
      case ArduinoIoTCloudEvent::CONNECT: callback_connect(); break;
      case ArduinoIoTCloudEvent::SYNC: callback_sync(); break;
      case ArduinoIoTCloudEvent::DISCONNECT: callback_disconnect(); break;
    }
  }

  if(lora_receive(&_rx_packet))
  {    
    //Fade white
    led_fade_reset();
    led_fade(1, 1, 1, 2, true, 5000);
    packet_fade_triggered = true;

    if(_rx_packet.data.temperature != -99)
      temperature = round_to_dp(((float)_rx_packet.data.temperature/100),2);

    if(_rx_packet.data.humidity != -99 && _rx_packet.data.humidity >= 0)
      humidity = (float)_rx_packet.data.humidity/100;

    if(_rx_packet.data.air_pressure != -99 && _rx_packet.data.air_pressure >= 0)
      pressure = _rx_packet.data.air_pressure;
    
    if(_rx_packet.data.air_pollution_level != -99 && _rx_packet.data.air_pollution_level >= 0)
      pollution = round_to_dp(((float)_rx_packet.data.air_pollution_level/100),2);

    if(_rx_packet.data.noise_level != -99)
      noiselevel = round_to_dp(((float)_rx_packet.data.noise_level/100),2);

    if(_rx_packet.data.radiation_level != -99 && _rx_packet.data.radiation_level >= 0)
      radiationlevel = round_to_dp(((float)_rx_packet.data.radiation_level/100),2);

    if(_rx_packet.data.wind_spd != -99 && _rx_packet.data.wind_spd >= 0)
      windspeed = round_to_dp(((float)_rx_packet.data.wind_spd/100),2);

    if(_rx_packet.data.wind_dir != -99)
    {
      switch(_rx_packet.data.wind_dir)
      {
        case 0: winddir = "N"; break;
        case 1: winddir = "NE"; break;
        case 2: winddir = "E"; break;
        case 3: winddir = "SE"; break;
        case 4: winddir = "S"; break;
        case 5: winddir = "SO"; break;
        case 6: winddir = "O"; break;
        case 7: winddir = "NO"; break;
        default: winddir = "UNK"; break;
      }
    }
  }
}