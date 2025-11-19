/* ------------------------- HEADER (.h) FILE INFORMATION ------------------------- */
/**
 * PROJECT:         ENV.EYE SPOT Project
 * 
 * FILE:            "wf183de_registers.h"
 * DESCRIPTION:     WF183DE ambient pressure and temperature sensor register codes.
 * 
 */

/* ------------------------- Register Codes ------------------------- */
#define WF183DE_I2C_ADDRESS 0x6D
#define WF183DE_CMD_REG 0x0A
#define WF183DE_PRESSURE_4_REG 0x0B
#define WF183DE_PRESSURE_3_REG 0x0C
#define WF183DE_PRESSURE_2_REG 0x0D
#define WF183DE_PRESSURE_1_REG 0x0E
#define WF183DE_TEMP_HI_REG 0x0F
#define WF183DE_TEMP_LO_REG 0x10
#define WF183DE_STATUS_REG 0x13

/* ------------------------- Command Options ------------------------- */
#define WF183DE_CMD_READ_TEMPERATURE 0x04
#define WF183DE_CMD_READ_PRESSURE 0x06
#define WF183DE_STATUS_READY 1
#define WF183DE_STATUS_CONVERTING 0

/* ------------------------- Custom Error Macros ------------------------- */
#define WF183DE_CONVERTING 0xFE