#pragma once
/* ------------------------- HEADER (.h) FILE INFORMATION ------------------------- */
/**
 * PROJECT:         ENV.EYE SPOT Project
 * 
 * FILE:            "syp16_registers.h"
 * DESCRIPTION:     SYP16 air quality sensor uart commands.
 * 
 */

#include "inttypes.h"

#define SYP16_CONVERTING 0xFE

/* ------------------------- UART Commands ------------------------- */
const uint8_t SET_ACTIVE_UPLOAD_MODE[9] = {0xFF, 0x00, 0x78, 0x40, 0x00, 0x00, 0x00, 0x00, 0x48};
const uint8_t SET_QUESTION_ANSWER_MODE[9] = {0xFF, 0x00, 0x78, 0x41, 0x00, 0x00, 0x00, 0x00, 0x47};
const uint8_t GET_GAS_CONCENTRATION[9] = {0xFF, 0x00, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7A};