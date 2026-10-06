#include "S32K144.h"

#include "can.h"
#include "ui.h"

#include <stdint.h>


/* =========================================================
 * DEBUG
 * ========================================================= */

volatile uint16_t rx_id = 0U;

volatile uint8_t rx_length = 0U;

volatile uint8_t rx_data0 = 0U;

volatile uint8_t rx_data1 = 0U;


/*
 * CAN:
 * 28.53C -> 2853
 */
volatile uint16_t rx_temperature_x100 = 0U;


/*
 * Debug float
 */
volatile float rx_temperature = 0.0f;


/*
 * Display:
 * 28.5C -> 285
 */
volatile uint16_t rx_temperature_x10 = 0U;


volatile uint32_t receive_count = 0U;


/* =========================================================
 * MAIN
 * ========================================================= */

int main(void)
{
    uint16_t id;

    uint8_t data[8];

    uint8_t length;


    /* =====================================================
     * UI
     * ===================================================== */

    UI_Init();


    /* =====================================================
     * CAN
     * ===================================================== */

    CAN_Init();


    if (can_last_status != CAN_OK)
    {
        while (1)
        {
        }
    }


    CAN_ReceiveInit(
            0x123U);


    /* =====================================================
     * LOOP
     * ===================================================== */

    while (1)
    {
        if (CAN_Receive(
                &id,
                data,
                &length))
        {
            rx_id =
                    id;


            rx_length =
                    length;


            if ((id == 0x123U) &&
                (length >= 2U))
            {
                /* =========================================
                 * DEBUG CAN
                 * ========================================= */

                rx_data0 =
                        data[0];


                rx_data1 =
                        data[1];


                /* =========================================
                 * GHÉP 2 BYTE
                 *
                 * 2853 -> 28.53C
                 * ========================================= */

                rx_temperature_x100 =
                        ((uint16_t)data[0] << 8)
                        |
                        ((uint16_t)data[1]);


                /* =========================================
                 * FLOAT DEBUG
                 * ========================================= */

                rx_temperature =
                        ((float)
                         rx_temperature_x100)
                        /
                        100.0f;


                /* =========================================
                 * x100 -> x10
                 *
                 * làm tròn:
                 *
                 * 28.54 -> 28.5
                 * 28.55 -> 28.6
                 * ========================================= */

                rx_temperature_x10 =
                        (rx_temperature_x100 +
                         5U)
                        /
                        10U;


                receive_count++;


                /* =========================================
                 * DISPLAY
                 * ========================================= */

                UI_UpdateTemperature(
                        rx_temperature_x10);
            }
        }
    }
}
