#pragma once
/* ------------------------- HEADER (.h) FILE INFORMATION ------------------------- */
/**
 * PROJECT:         ENV.EYE SPOT Project
 * 
 * FILE:            "env_eye_config.h"
 * DESCRIPTION:     Eve.Eye SPOT configuration options.
 * 
 */

#define SPOT_MODULE_ID 0
#define SLEEP_TIME_US 30*1000000//15*60*1000000 //15 Min.

#define MOUNT_DIRECTION N
#define WIND_DIR_STEP_DELTA 50
const int WIND_DIR_CALIBRATION[8] =
{
    0,      //W
    540,    //SW
    1085,   //S
    1650,   //SE
    2220,   //E
    2800,   //NE
    3435,   //N
    4095    //NW
};

#define MIC_DC_OFFSET 2160
#define MIC_SAMPLE_NUM 50
#define MIC_DB_OFFSET 5

#define GEIGER_USV_HOUR_PER_PULSE_S 0.39 


