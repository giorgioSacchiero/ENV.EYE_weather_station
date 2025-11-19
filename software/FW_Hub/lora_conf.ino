/* ------------------------- C/C++ (.ino) FILE INFORMATION ------------------------- */
/**
 * PROJECT:         ENV.EYE HUB Project
 * 
 * FILE:            "lora_conf.ino"
 * 
 */

#include <LoRa.h>
#include "lora_conf.h"

void lora_begin()
{
  if (!LoRa.begin(LORA_COMM_FREQ)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  LoRa.setSPIFrequency(LORA_SPI_FREQ);
  LoRa.setPins(LORA_CS_PIN, LORA_RESET_PIN, LORA_INTR_PIN);
  LoRa.setSignalBandwidth(LORA_BANDWIDTH);
  LoRa.setSpreadingFactor(LORA_SPREADING_FACTOR);
  LoRa.setCodingRate4(LORA_CODING_RATE);

  #ifdef LORA_ENABLE_CRC
    LoRa.enableCrc();
  #endif
}

bool lora_receive(lora_packet_t *_data)
{
  int packetSize = LoRa.parsePacket();
  if (packetSize) 
  {
    uint8_t i = 0;
    
    // read packet
    while (LoRa.available())     
      _data->raw_payload[i++] = LoRa.read();

    _data->data.signal_power = LoRa.packetRssi();
    return true;
  }
  return false;
}