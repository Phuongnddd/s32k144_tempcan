#include "spi.h"


#define SPI_SCK_PIN       2U
#define SPI_MOSI_PIN      4U


#define SCK_HIGH()   \
    (PTB->PSOR = (1UL << SPI_SCK_PIN))

#define SCK_LOW()    \
    (PTB->PCOR = (1UL << SPI_SCK_PIN))


#define MOSI_HIGH()  \
    (PTB->PSOR = (1UL << SPI_MOSI_PIN))

#define MOSI_LOW()   \
    (PTB->PCOR = (1UL << SPI_MOSI_PIN))


void SPI_DelayShort(void)
{
    volatile uint32_t i;


    for (i = 0U;
         i < 10U;
         i++)
    {
        __asm("nop");
    }
}


void SPI_Init(void)
{
    PCC->PCCn[PCC_PORTB_INDEX] |=
            PCC_PCCn_CGC_MASK;


    /* PTB2 SCK */
    PORTB->PCR[SPI_SCK_PIN] =
            PORT_PCR_MUX(1);


    /* PTB4 MOSI */
    PORTB->PCR[SPI_MOSI_PIN] =
            PORT_PCR_MUX(1);


    PTB->PDDR |=
            (1UL << SPI_SCK_PIN) |
            (1UL << SPI_MOSI_PIN);


    SCK_LOW();

    MOSI_LOW();
}


void SPI_WriteByte(
        uint8_t data)
{
    uint8_t i;


    for (i = 0U;
         i < 8U;
         i++)
    {
        SCK_LOW();


        if ((data & 0x80U) != 0U)
        {
            MOSI_HIGH();
        }
        else
        {
            MOSI_LOW();
        }


        SPI_DelayShort();


        SCK_HIGH();


        SPI_DelayShort();


        data <<= 1;
    }


    SCK_LOW();


    SPI_DelayShort();
}
