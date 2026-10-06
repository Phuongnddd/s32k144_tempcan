#ifndef UI_H_
#define UI_H_

#include "S32K144.h"
#include <stdint.h>


/* =========================================================
 * TEMPERATURE LIMIT
 *
 * 105.0 C -> 1050
 * ========================================================= */

#define TEMP_LIMIT_X10     1050U


void UI_Init(void);

void UI_UpdateTemperature(
        uint16_t temperature_x10);


#endif
