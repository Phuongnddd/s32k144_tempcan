#include "can.h"


#define CAN_TX_MB           0U
#define CAN_RX_MB           4U

#define CAN_MAX_MB          15U

#define CAN_TIMEOUT_VALUE   1000000UL


volatile uint32_t can_step = 0U;

volatile uint32_t can_mcr = 0U;
volatile uint32_t can_ctrl1 = 0U;
volatile uint32_t can_esr1 = 0U;
volatile uint32_t can_iflag1 = 0U;

volatile uint32_t can_tx_count = 0U;
volatile uint32_t can_rx_count = 0U;

volatile uint32_t can_error_count = 0U;

volatile uint8_t can_last_status = CAN_ERROR;


/* =========================================================
 * CAN CLOCK
 *
 * S32K144 EVB:
 * External crystal = 8 MHz
 * SOSCDIV2 = 8 MHz
 * ========================================================= */

static uint8_t CAN_Clock_Init(void)
{
    uint32_t timeout;


    can_step = 1U;


    if ((SCG->SOSCCSR &
         SCG_SOSCCSR_SOSCVLD_MASK) != 0U)
    {
        return CAN_OK;
    }


    SCG->SOSCCSR &=
            ~SCG_SOSCCSR_SOSCEN_MASK;


    SCG->SOSCDIV =
            SCG_SOSCDIV_SOSCDIV1(1U) |
            SCG_SOSCDIV_SOSCDIV2(1U);


    SCG->SOSCCFG =
            SCG_SOSCCFG_RANGE(2U) |
            SCG_SOSCCFG_EREFS_MASK;


    SCG->SOSCCSR |=
            SCG_SOSCCSR_SOSCEN_MASK;


    timeout =
            CAN_TIMEOUT_VALUE;


    while (((SCG->SOSCCSR &
             SCG_SOSCCSR_SOSCVLD_MASK) == 0U) &&
           (timeout > 0U))
    {
        timeout--;
    }


    if (timeout == 0U)
    {
        can_step = 101U;

        return CAN_ERROR;
    }


    return CAN_OK;
}


/* =========================================================
 * PTE4 = CAN RX
 * PTE5 = CAN TX
 * ========================================================= */

static void CAN_Pin_Init(void)
{
    PCC->PCCn[PCC_PORTE_INDEX] |=
            PCC_PCCn_CGC_MASK;


    /* RX */
    PORTE->PCR[4] =
            PORT_PCR_MUX(5U);


    /* TX */
    PORTE->PCR[5] =
            PORT_PCR_MUX(5U);
}


/* =========================================================
 * CAN INIT
 *
 * 500 kbps
 * 8 MHz
 * 16 TQ
 * ========================================================= */

void CAN_Init(void)
{
    uint32_t i;

    uint32_t timeout;


    can_last_status =
            CAN_ERROR;


    can_step = 0U;


    if (CAN_Clock_Init() != CAN_OK)
    {
        return;
    }


    CAN_Pin_Init();


    can_step = 3U;


    /* Enable FlexCAN0 clock */
    PCC->PCCn[PCC_FlexCAN0_INDEX] |=
            PCC_PCCn_CGC_MASK;


    /*
     * SOSCDIV2
     */
    CAN0->CTRL1 &=
            ~CAN_CTRL1_CLKSRC_MASK;


    /* Enable module */
    CAN0->MCR &=
            ~CAN_MCR_MDIS_MASK;


    timeout =
            CAN_TIMEOUT_VALUE;


    while (((CAN0->MCR &
             CAN_MCR_LPMACK_MASK) != 0U) &&
           (timeout > 0U))
    {
        timeout--;
    }


    if (timeout == 0U)
    {
        can_step = 102U;

        return;
    }


    /* Freeze mode */
    CAN0->MCR |=
            CAN_MCR_FRZ_MASK;


    CAN0->MCR |=
            CAN_MCR_HALT_MASK;


    timeout =
            CAN_TIMEOUT_VALUE;


    while (((CAN0->MCR &
             CAN_MCR_FRZACK_MASK) == 0U) &&
           (timeout > 0U))
    {
        timeout--;
    }


    if (timeout == 0U)
    {
        can_step = 103U;

        return;
    }


    /* Classical CAN */
    CAN0->MCR &=
            ~CAN_MCR_FDEN_MASK;


    /* Disable RX FIFO */
    CAN0->MCR &=
            ~CAN_MCR_RFEN_MASK;


    CAN0->MCR &=
            ~CAN_MCR_MAXMB_MASK;


    CAN0->MCR |=
            CAN_MCR_MAXMB(CAN_MAX_MB) |
            CAN_MCR_SRXDIS_MASK;


    /*
     * 8 MHz / 16 TQ
     * = 500 kbps
     */
    CAN0->CTRL1 =
            CAN_CTRL1_PRESDIV(0U) |
            CAN_CTRL1_RJW(2U) |
            CAN_CTRL1_PSEG1(6U) |
            CAN_CTRL1_PSEG2(2U) |
            CAN_CTRL1_PROPSEG(4U);


    CAN0->CTRL1 &=
            ~CAN_CTRL1_CLKSRC_MASK;


    /*
     * Clear message buffer RAM
     */
    for (i = 0U;
         i < 128U;
         i++)
    {
        CAN0->RAMn[i] = 0U;
    }


    /*
     * Exact ID matching
     */
    CAN0->RXMGMASK =
            0xFFFFFFFFUL;


    /*
     * TX MB0 inactive
     */
    CAN0->RAMn[0] =
            (0x8UL << 24);


    /*
     * Clear interrupt flags
     */
    CAN0->IFLAG1 =
            0xFFFFFFFFUL;


    /* Exit Freeze */
    CAN0->MCR &=
            ~CAN_MCR_HALT_MASK;


    timeout =
            CAN_TIMEOUT_VALUE;


    while (((CAN0->MCR &
             CAN_MCR_FRZACK_MASK) != 0U) &&
           (timeout > 0U))
    {
        timeout--;
    }


    if (timeout == 0U)
    {
        can_step = 104U;

        return;
    }


    /*
     * Wait ready
     */
    timeout =
            CAN_TIMEOUT_VALUE;


    while (((CAN0->MCR &
             CAN_MCR_NOTRDY_MASK) != 0U) &&
           (timeout > 0U))
    {
        timeout--;
    }


    if (timeout == 0U)
    {
        can_step = 105U;

        return;
    }


    can_mcr =
            CAN0->MCR;

    can_ctrl1 =
            CAN0->CTRL1;

    can_esr1 =
            CAN0->ESR1;


    can_last_status =
            CAN_OK;


    can_step = 11U;
}


/* =========================================================
 * CAN SEND
 *
 * Standard ID
 * DLC 0..8
 * ========================================================= */

uint8_t CAN_Send(
        uint16_t id,
        const uint8_t *data,
        uint8_t length)
{
    uint32_t word0 = 0U;

    uint32_t word1 = 0U;

    uint32_t timeout;

    uint8_t i;


    if (data == 0)
    {
        return CAN_ERROR;
    }


    if (length > 8U)
    {
        return CAN_ERROR;
    }


    if (id > 0x7FFU)
    {
        return CAN_ERROR;
    }


    /*
     * Clear MB0 flag
     */
    CAN0->IFLAG1 =
            (1UL << CAN_TX_MB);


    /*
     * TX inactive
     */
    CAN0->RAMn[0] =
            (0x8UL << 24);


    /*
     * Standard ID
     */
    CAN0->RAMn[1] =
            ((uint32_t)id << 18);


    /*
     * Data byte 0..3
     */
    for (i = 0U;
         (i < length) &&
         (i < 4U);
         i++)
    {
        word0 |=
                ((uint32_t)data[i] <<
                 (24U -
                  ((uint32_t)i * 8U)));
    }


    /*
     * Data byte 4..7
     */
    for (i = 4U;
         i < length;
         i++)
    {
        word1 |=
                ((uint32_t)data[i] <<
                 (24U -
                  (((uint32_t)i -
                    4U) * 8U)));
    }


    CAN0->RAMn[2] =
            word0;


    CAN0->RAMn[3] =
            word1;


    /*
     * CODE = 0xC
     * TX DATA
     */
    CAN0->RAMn[0] =
            (0xCUL << 24) |
            ((uint32_t)length << 16);


    timeout =
            CAN_TIMEOUT_VALUE;


    while (((CAN0->IFLAG1 &
             (1UL << CAN_TX_MB)) == 0U) &&
           (timeout > 0U))
    {
        timeout--;
    }


    can_esr1 =
            CAN0->ESR1;


    if (timeout == 0U)
    {
        can_error_count++;

        can_last_status =
                CAN_TIMEOUT;

        return CAN_TIMEOUT;
    }


    CAN0->IFLAG1 =
            (1UL << CAN_TX_MB);


    can_tx_count++;


    can_last_status =
            CAN_OK;


    return CAN_OK;
}


/* =========================================================
 * RX INIT
 *
 * MB4
 * ========================================================= */

void CAN_ReceiveInit(
        uint16_t id)
{
    uint32_t base;


    base =
            CAN_RX_MB * 4U;


    /*
     * RX inactive
     */
    CAN0->RAMn[base + 0U] =
            0U;


    /*
     * Standard ID
     */
    CAN0->RAMn[base + 1U] =
            ((uint32_t)id << 18);


    CAN0->RAMn[base + 2U] =
            0U;

    CAN0->RAMn[base + 3U] =
            0U;


    /*
     * CODE = 0x4
     * RX EMPTY
     */
    CAN0->RAMn[base + 0U] =
            (0x4UL << 24);


    /*
     * Clear flag
     */
    CAN0->IFLAG1 =
            (1UL << CAN_RX_MB);
}


/* =========================================================
 * CAN RECEIVE
 *
 * return 1 = new frame
 * return 0 = no frame
 * ========================================================= */

uint8_t CAN_Receive(
        uint16_t *id,
        uint8_t *data,
        uint8_t *length)
{
    uint32_t base;

    uint32_t cs;

    uint32_t word0;

    uint32_t word1;

    uint32_t timer;

    uint8_t i;


    if ((CAN0->IFLAG1 &
         (1UL << CAN_RX_MB)) == 0U)
    {
        return 0U;
    }


    base =
            CAN_RX_MB * 4U;


    /*
     * Read CS first
     */
    cs =
            CAN0->RAMn[base + 0U];


    /*
     * Standard ID
     */
    *id =
            (uint16_t)
            ((CAN0->RAMn[base + 1U]
              >> 18)
             &
             0x7FFU);


    word0 =
            CAN0->RAMn[base + 2U];


    word1 =
            CAN0->RAMn[base + 3U];


    /*
     * Read TIMER to unlock MB
     */
    timer =
            CAN0->TIMER;

    (void)timer;


    /*
     * DLC
     */
    *length =
            (uint8_t)
            ((cs >> 16) &
             0x0FU);


    if (*length > 8U)
    {
        *length = 8U;
    }


    /*
     * bytes 0..3
     */
    for (i = 0U;
         (i < *length) &&
         (i < 4U);
         i++)
    {
        data[i] =
                (uint8_t)
                (word0 >>
                 (24U -
                  ((uint32_t)i *
                   8U)));
    }


    /*
     * bytes 4..7
     */
    for (i = 4U;
         i < *length;
         i++)
    {
        data[i] =
                (uint8_t)
                (word1 >>
                 (24U -
                  (((uint32_t)i -
                    4U) *
                   8U)));
    }


    /*
     * Clear RX flag
     */
    CAN0->IFLAG1 =
            (1UL << CAN_RX_MB);


    can_rx_count++;


    can_esr1 =
            CAN0->ESR1;


    return 1U;
}
