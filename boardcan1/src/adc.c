#include "adc.h"


#define ADC_TIMEOUT     1000000UL


/* =========================================================
 * DEBUG VARIABLES
 * ========================================================= */

/*
 * Giá trị ADC raw
 *
 * ADC 12-bit:
 * 0 -> 4095
 */
volatile uint16_t adc_debug_raw =
        0U;


/*
 * Điện áp tính được từ ADC
 *
 * Ví dụ:
 * 0.250 V
 */
volatile float adc_debug_voltage =
        0.0f;


/*
 * Nhiệt độ LM35
 *
 * Ví dụ:
 * 25.00 C
 */
volatile float lm35_debug_temperature =
        0.0f;


/*
 * Nhiệt độ x100
 *
 * Ví dụ:
 * 25.35 C -> 2535
 */
volatile uint16_t lm35_debug_temperature_x100 =
        0U;


/* =========================================================
 * ADC INIT
 * ========================================================= */

uint8_t ADC_Init(void)
{
    uint32_t timeout =
            ADC_TIMEOUT;


    /* =====================================================
     * PORTC CLOCK
     * ===================================================== */

    PCC->PCCn[PCC_PORTC_INDEX] |=
            PCC_PCCn_CGC_MASK;


    /* =====================================================
     * PTC3 = ANALOG
     * ===================================================== */

    PORTC->PCR[3] =
            0U;


    /* =====================================================
     * WAIT FIRC READY
     * ===================================================== */

    while ((SCG->FIRCCSR &
            SCG_FIRCCSR_FIRCVLD_MASK)
            == 0U)
    {
        if (timeout == 0U)
        {
            return ADC_ERROR;
        }


        timeout--;
    }


    /* =====================================================
     * FIRCDIV2
     *
     * FIRC = 48 MHz
     * /2 -> 24 MHz
     * ===================================================== */

    SCG->FIRCDIV =
            (
                SCG->FIRCDIV &
                ~SCG_FIRCDIV_FIRCDIV2_MASK
            )
            |
            SCG_FIRCDIV_FIRCDIV2(2U);


    /* =====================================================
     * ADC CLOCK
     * ===================================================== */

    PCC->PCCn[PCC_ADC0_INDEX] &=
            ~PCC_PCCn_CGC_MASK;


    PCC->PCCn[PCC_ADC0_INDEX] =
            PCC_PCCn_PCS(3U)
            |
            PCC_PCCn_CGC_MASK;


    /* =====================================================
     * ADC CONFIG
     *
     * 12-bit
     *
     * ADCK:
     * 24 MHz / 2 = 12 MHz
     * ===================================================== */

    ADC0->CFG1 =
            ADC_CFG1_ADICLK(0U)
            |
            ADC_CFG1_MODE(1U)
            |
            ADC_CFG1_ADIV(1U);


    /* =====================================================
     * SAMPLE TIME
     * ===================================================== */

    ADC0->CFG2 =
            ADC_CFG2_SMPLTS(12U);


    /* =====================================================
     * SOFTWARE TRIGGER
     * ===================================================== */

    ADC0->SC2 =
            0U;


    ADC0->SC3 =
            0U;


    /* =====================================================
     * DISABLE CHANNEL
     * ===================================================== */

    ADC0->SC1[0] =
            ADC_SC1_ADCH(0x1FU);


    return ADC_OK;
}


/* =========================================================
 * ADC READ RAW
 * ========================================================= */

uint16_t ADC_Read(
        uint8_t channel)
{
    uint32_t timeout =
            ADC_TIMEOUT;


    channel &=
            0x1FU;


    /* =====================================================
     * START CONVERSION
     * ===================================================== */

    ADC0->SC1[0] =
            ADC_SC1_ADCH(
                    channel);


    /* =====================================================
     * WAIT COMPLETE
     * ===================================================== */

    while ((ADC0->SC1[0] &
            ADC_SC1_COCO_MASK)
            == 0U)
    {
        if (timeout == 0U)
        {
            adc_debug_raw =
                    0U;

            return 0U;
        }


        timeout--;
    }


    /* =====================================================
     * READ RESULT
     * ===================================================== */

    adc_debug_raw =
            (uint16_t)
            ADC0->R[0];


    return adc_debug_raw;
}


/* =========================================================
 * ADC READ VOLTAGE
 *
 * V = ADCraw * Vref / 4095
 * ========================================================= */

float ADC_ReadVoltage(
        uint8_t channel)
{
    uint16_t value;


    value =
            ADC_Read(
                    channel);


    adc_debug_voltage =
            ((float)value *
             ADC_VREF)
            /
            ADC_MAX_VALUE;


    return adc_debug_voltage;
}


/* =========================================================
 * LM35 READ TEMPERATURE
 *
 * LM35:
 *
 * 10 mV / C
 *
 * T = Voltage * 100
 *
 * Ví dụ:
 * 0.250 V -> 25.0 C
 * ========================================================= */

float LM35_ReadTemperature(void)
{
    float voltage;


    voltage =
            ADC_ReadVoltage(
                    ADC_CHANNEL_PTC3);


    lm35_debug_temperature =
            voltage *
            100.0f;


    return lm35_debug_temperature;
}


/* =========================================================
 * LM35 READ TEMPERATURE x100
 *
 * Ví dụ:
 *
 * 28.53 C -> 2853
 * ========================================================= */

uint16_t LM35_ReadTemperatureX100(void)
{
    float temperature;


    temperature =
            LM35_ReadTemperature();


    lm35_debug_temperature_x100 =
            (uint16_t)
            (
                temperature *
                100.0f
                +
                0.5f
            );


    return lm35_debug_temperature_x100;
}
