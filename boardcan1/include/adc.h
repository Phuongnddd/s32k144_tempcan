#ifndef ADC_H_
#define ADC_H_

#include "S32K144.h"
#include <stdint.h>


/* =========================================================
 * STATUS
 * ========================================================= */

#define ADC_OK                  0U
#define ADC_ERROR               1U


/* =========================================================
 * ADC CHANNEL
 *
 * PTC3 = ADC0 channel 11
 * ========================================================= */

#define ADC_CHANNEL_PTC3        11U


/* =========================================================
 * ADC CONFIG
 *
 * ADC 12-bit:
 *
 * 0 -> 4095
 * ========================================================= */

#define ADC_VREF                3.3f
#define ADC_MAX_VALUE           4095.0f


/* =========================================================
 * ADC
 * ========================================================= */

uint8_t ADC_Init(void);

uint16_t ADC_Read(
        uint8_t channel);

float ADC_ReadVoltage(
        uint8_t channel);


/* =========================================================
 * LM35
 * ========================================================= */

/*
 * Trả về nhiệt độ dạng float
 *
 * Ví dụ:
 * 28.53 C
 */
float LM35_ReadTemperature(void);


/*
 * Trả về nhiệt độ x100
 *
 * Ví dụ:
 *
 * 28.53 C -> 2853
 *
 * Dùng để gửi CAN
 */
uint16_t LM35_ReadTemperatureX100(void);


#endif
