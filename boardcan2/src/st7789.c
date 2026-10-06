#include "S32K144.h"

#include "spi.h"
#include "st7789.h"


#define LCD_CS_PIN       5U
#define LCD_DC_PIN       12U
#define LCD_RST_PIN      13U


#define CS_HIGH() \
    (PTB->PSOR = (1UL << LCD_CS_PIN))

#define CS_LOW() \
    (PTB->PCOR = (1UL << LCD_CS_PIN))


#define DC_HIGH() \
    (PTC->PSOR = (1UL << LCD_DC_PIN))

#define DC_LOW() \
    (PTC->PCOR = (1UL << LCD_DC_PIN))


#define RST_HIGH() \
    (PTC->PSOR = (1UL << LCD_RST_PIN))

#define RST_LOW() \
    (PTC->PCOR = (1UL << LCD_RST_PIN))


/* =========================================================
 * DELAY
 * ========================================================= */

static void delay_ms(uint32_t ms)
{
    volatile uint32_t i;

    while (ms--)
    {
        for (i = 0U;
             i < 12000U;
             i++)
        {
            __asm("nop");
        }
    }
}


/* =========================================================
 * GPIO INIT
 * ========================================================= */

static void LCD_GPIO_Init(void)
{
    PCC->PCCn[PCC_PORTB_INDEX] |=
            PCC_PCCn_CGC_MASK;

    PCC->PCCn[PCC_PORTC_INDEX] |=
            PCC_PCCn_CGC_MASK;


    /* CS = PTB5 */
    PORTB->PCR[LCD_CS_PIN] =
            PORT_PCR_MUX(1);

    PTB->PDDR |=
            (1UL << LCD_CS_PIN);


    /* DC = PTC12 */
    PORTC->PCR[LCD_DC_PIN] =
            PORT_PCR_MUX(1);


    /* RST = PTC13 */
    PORTC->PCR[LCD_RST_PIN] =
            PORT_PCR_MUX(1);


    PTC->PDDR |=
            (1UL << LCD_DC_PIN)
            |
            (1UL << LCD_RST_PIN);


    CS_HIGH();

    DC_HIGH();

    RST_HIGH();
}


/* =========================================================
 * COMMAND
 * ========================================================= */

static void ST7789_Command(
        uint8_t command)
{
    CS_LOW();

    SPI_DelayShort();


    DC_LOW();

    SPI_DelayShort();


    SPI_WriteByte(
            command);


    SPI_DelayShort();


    CS_HIGH();

    SPI_DelayShort();
}


/* =========================================================
 * DATA
 * ========================================================= */

static void ST7789_Data(
        uint8_t data)
{
    CS_LOW();

    SPI_DelayShort();


    DC_HIGH();

    SPI_DelayShort();


    SPI_WriteByte(
            data);


    SPI_DelayShort();


    CS_HIGH();

    SPI_DelayShort();
}


/* =========================================================
 * INIT
 * ========================================================= */

void ST7789_Init(void)
{
    SPI_Init();

    LCD_GPIO_Init();


    /* Hardware Reset */

    RST_HIGH();

    delay_ms(10);


    RST_LOW();

    delay_ms(50);


    RST_HIGH();

    delay_ms(150);


    /* Software Reset */

    ST7789_Command(
            0x01);

    delay_ms(150);


    /* Sleep Out */

    ST7789_Command(
            0x11);

    delay_ms(150);


    /* RGB565 */

    ST7789_Command(
            0x3A);

    ST7789_Data(
            0x55);


    /* MADCTL */

    ST7789_Command(
            0x36);

    ST7789_Data(
            0x00);


    /* Normal mode */

    ST7789_Command(
            0x13);


    /* Inversion ON */

    ST7789_Command(
            0x21);


    /* Display ON */

    ST7789_Command(
            0x29);


    delay_ms(100);
}


/* =========================================================
 * SET WINDOW
 * ========================================================= */

static void ST7789_SetWindow(
        uint16_t x0,
        uint16_t y0,
        uint16_t x1,
        uint16_t y1)
{
    /* Column */

    ST7789_Command(
            0x2A);


    ST7789_Data(
            (uint8_t)(x0 >> 8));

    ST7789_Data(
            (uint8_t)x0);


    ST7789_Data(
            (uint8_t)(x1 >> 8));

    ST7789_Data(
            (uint8_t)x1);


    /* Row */

    ST7789_Command(
            0x2B);


    ST7789_Data(
            (uint8_t)(y0 >> 8));

    ST7789_Data(
            (uint8_t)y0);


    ST7789_Data(
            (uint8_t)(y1 >> 8));

    ST7789_Data(
            (uint8_t)y1);


    /* RAM Write */

    ST7789_Command(
            0x2C);
}


/* =========================================================
 * FILL RECT
 * ========================================================= */

void ST7789_FillRect(
        int16_t x,
        int16_t y,
        int16_t width,
        int16_t height,
        uint16_t color)
{
    uint32_t count;

    uint8_t high;
    uint8_t low;


    if ((width <= 0) ||
        (height <= 0))
    {
        return;
    }


    if ((x < 0) ||
        (y < 0))
    {
        return;
    }


    if ((x >= ST7789_WIDTH) ||
        (y >= ST7789_HEIGHT))
    {
        return;
    }


    if ((x + width) >
        ST7789_WIDTH)
    {
        width =
            ST7789_WIDTH - x;
    }


    if ((y + height) >
        ST7789_HEIGHT)
    {
        height =
            ST7789_HEIGHT - y;
    }


    ST7789_SetWindow(
            (uint16_t)x,
            (uint16_t)y,
            (uint16_t)(x + width - 1),
            (uint16_t)(y + height - 1));


    high =
        (uint8_t)(color >> 8);


    low =
        (uint8_t)(color & 0xFFU);


    count =
        (uint32_t)width *
        (uint32_t)height;


    CS_LOW();

    DC_HIGH();


    while (count--)
    {
        SPI_WriteByte(high);

        SPI_WriteByte(low);
    }


    CS_HIGH();
}


/* =========================================================
 * FILL SCREEN
 * ========================================================= */

void ST7789_FillScreen(
        uint16_t color)
{
    ST7789_FillRect(
            0,
            0,
            ST7789_WIDTH,
            ST7789_HEIGHT,
            color);
}


/* =========================================================
 * FONT
 * ========================================================= */

static uint8_t FontColumn(
        char c,
        uint8_t column)
{
    static const uint8_t SPACE[5] =
        {0x00,0x00,0x00,0x00,0x00};


    /* =====================================================
     * NUMBERS
     * ===================================================== */

    static const uint8_t N0[5] =
        {0x3E,0x51,0x49,0x45,0x3E};

    static const uint8_t N1[5] =
        {0x00,0x42,0x7F,0x40,0x00};

    static const uint8_t N2[5] =
        {0x42,0x61,0x51,0x49,0x46};

    static const uint8_t N3[5] =
        {0x21,0x41,0x45,0x4B,0x31};

    static const uint8_t N4[5] =
        {0x18,0x14,0x12,0x7F,0x10};

    static const uint8_t N5[5] =
        {0x27,0x45,0x45,0x45,0x39};

    static const uint8_t N6[5] =
        {0x3C,0x4A,0x49,0x49,0x30};

    static const uint8_t N7[5] =
        {0x01,0x71,0x09,0x05,0x03};

    static const uint8_t N8[5] =
        {0x36,0x49,0x49,0x49,0x36};

    static const uint8_t N9[5] =
        {0x06,0x49,0x49,0x29,0x1E};


    /* dấu chấm */
    static const uint8_t DOT[5] =
        {0x00,0x60,0x60,0x00,0x00};


    /* =====================================================
     * LETTERS
     * ===================================================== */

    static const uint8_t A[5] =
        {0x7E,0x11,0x11,0x11,0x7E};

    static const uint8_t C[5] =
        {0x3E,0x41,0x41,0x41,0x22};

    static const uint8_t D[5] =
        {0x7F,0x41,0x41,0x22,0x1C};

    static const uint8_t E[5] =
        {0x7F,0x49,0x49,0x49,0x41};

    static const uint8_t G[5] =
        {0x3E,0x41,0x49,0x49,0x7A};

    static const uint8_t I[5] =
        {0x00,0x41,0x7F,0x41,0x00};

    static const uint8_t L[5] =
        {0x7F,0x40,0x40,0x40,0x40};

    static const uint8_t M[5] =
        {0x7F,0x02,0x0C,0x02,0x7F};

    static const uint8_t N[5] =
        {0x7F,0x04,0x08,0x10,0x7F};

    static const uint8_t O[5] =
        {0x3E,0x41,0x41,0x41,0x3E};

    static const uint8_t P[5] =
        {0x7F,0x09,0x09,0x09,0x06};

    static const uint8_t R[5] =
        {0x7F,0x09,0x19,0x29,0x46};

    static const uint8_t T[5] =
        {0x01,0x01,0x7F,0x01,0x01};

    static const uint8_t V[5] =
        {0x1F,0x20,0x40,0x20,0x1F};

    static const uint8_t W[5] =
        {0x7F,0x20,0x18,0x20,0x7F};


    const uint8_t *glyph =
            SPACE;


    if (column >= 5U)
    {
        return 0U;
    }


    switch (c)
    {
        case '0':
            glyph = N0;
            break;

        case '1':
            glyph = N1;
            break;

        case '2':
            glyph = N2;
            break;

        case '3':
            glyph = N3;
            break;

        case '4':
            glyph = N4;
            break;

        case '5':
            glyph = N5;
            break;

        case '6':
            glyph = N6;
            break;

        case '7':
            glyph = N7;
            break;

        case '8':
            glyph = N8;
            break;

        case '9':
            glyph = N9;
            break;


        case '.':
            glyph = DOT;
            break;


        case 'A':
            glyph = A;
            break;

        case 'C':
            glyph = C;
            break;

        case 'D':
            glyph = D;
            break;

        case 'E':
            glyph = E;
            break;

        case 'G':
            glyph = G;
            break;

        case 'I':
            glyph = I;
            break;

        case 'L':
            glyph = L;
            break;

        case 'M':
            glyph = M;
            break;

        case 'N':
            glyph = N;
            break;

        case 'O':
            glyph = O;
            break;

        case 'P':
            glyph = P;
            break;

        case 'R':
            glyph = R;
            break;

        case 'T':
            glyph = T;
            break;

        case 'V':
            glyph = V;
            break;

        case 'W':
            glyph = W;
            break;


        default:
            glyph = SPACE;
            break;
    }


    return glyph[column];
}


/* =========================================================
 * DRAW CHAR
 * ========================================================= */

static void ST7789_DrawChar(
        int16_t x,
        int16_t y,
        char c,
        uint16_t color,
        uint8_t scale)
{
    uint8_t col;
    uint8_t row;
    uint8_t bits;


    if (scale == 0U)
    {
        scale = 1U;
    }


    for (col = 0U;
         col < 5U;
         col++)
    {
        bits =
            FontColumn(
                    c,
                    col);


        for (row = 0U;
             row < 7U;
             row++)
        {
            if ((bits >> row) &
                0x01U)
            {
                ST7789_FillRect(
                        x + col * scale,
                        y + row * scale,
                        scale,
                        scale,
                        color);
            }
        }
    }
}


/* =========================================================
 * DRAW STRING NORMAL
 * ========================================================= */

void ST7789_DrawString(
        int16_t x,
        int16_t y,
        const char *text,
        uint16_t color,
        uint8_t scale)
{
    while (*text != '\0')
    {
        ST7789_DrawChar(
                x,
                y,
                *text,
                color,
                scale);


        x +=
            6 * scale;


        text++;
    }
}


/* =========================================================
 * DRAW CHAR + BACKGROUND
 *
 * Không cần clear trước
 * ========================================================= */

static void ST7789_DrawCharBG(
        int16_t x,
        int16_t y,
        char c,
        uint16_t color,
        uint16_t background,
        uint8_t scale)
{
    uint8_t col;
    uint8_t row;
    uint8_t bits;


    if (scale == 0U)
    {
        scale = 1U;
    }


    for (col = 0U;
         col < 5U;
         col++)
    {
        bits =
            FontColumn(
                    c,
                    col);


        for (row = 0U;
             row < 7U;
             row++)
        {
            if ((bits >> row) &
                0x01U)
            {
                ST7789_FillRect(
                        x + col * scale,
                        y + row * scale,
                        scale,
                        scale,
                        color);
            }
            else
            {
                ST7789_FillRect(
                        x + col * scale,
                        y + row * scale,
                        scale,
                        scale,
                        background);
            }
        }
    }


    /*
     * Xóa cột spacing
     */
    ST7789_FillRect(
            x + 5 * scale,
            y,
            scale,
            7 * scale,
            background);
}


/* =========================================================
 * DRAW STRING + BACKGROUND
 * ========================================================= */

void ST7789_DrawStringBG(
        int16_t x,
        int16_t y,
        const char *text,
        uint16_t color,
        uint16_t background,
        uint8_t scale)
{
    while (*text != '\0')
    {
        ST7789_DrawCharBG(
                x,
                y,
                *text,
                color,
                background,
                scale);


        x +=
            6 * scale;


        text++;
    }
}
