#ifndef CAN_H_
#define CAN_H_

#include "S32K144.h"
#include <stdint.h>

/* =========================================================
 * STATUS
 * ========================================================= */

#define CAN_OK          0U
#define CAN_ERROR       1U
#define CAN_TIMEOUT     2U


/* =========================================================
 * DEBUG VARIABLES
 * ========================================================= */

extern volatile uint32_t can_step;

extern volatile uint32_t can_mcr;
extern volatile uint32_t can_ctrl1;
extern volatile uint32_t can_esr1;
extern volatile uint32_t can_iflag1;

extern volatile uint32_t can_tx_count;
extern volatile uint32_t can_rx_count;
extern volatile uint32_t can_error_count;

extern volatile uint8_t can_last_status;


/* =========================================================
 * FUNCTIONS
 * ========================================================= */

void CAN_Init(void);

uint8_t CAN_Send(
        uint16_t id,
        const uint8_t *data,
        uint8_t length);

void CAN_ReceiveInit(
        uint16_t id);

uint8_t CAN_Receive(
        uint16_t *id,
        uint8_t *data,
        uint8_t *length);

#endif
