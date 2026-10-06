#include "ui.h"
#include "st7789.h"

#include <stdint.h>


/* =========================================================
 * COLOR
 * ========================================================= */

#define UI_BLACK        0x0000U
#define UI_WHITE        0xFFFFU
#define UI_GRAY         0x8410U
#define UI_DARK_GRAY    0x4208U
#define UI_RED          0xF800U
#define UI_DARK_RED     0x7800U


/* =========================================================
 * INTERNAL VARIABLES
 * ========================================================= */

static uint16_t displayed_temperature_x10 =
        0xFFFFU;

static uint8_t displayed_warning =
        0xFFU;


/* =========================================================
 * SHORT DELAY
 * ========================================================= */

static void UI_DelayShort(void)
{
    volatile uint32_t i;

    for (i = 0U;
         i < 12000U;
         i++)
    {
        __asm("nop");
    }
}


/* =========================================================
 * DEGREE SYMBOL
 * ========================================================= */

static void UI_DrawDegree(
        int16_t x,
        int16_t y,
        uint16_t color)
{
    ST7789_FillRect(
            x + 2,
            y,
            5,
            1,
            color);

    ST7789_FillRect(
            x,
            y + 2,
            1,
            5,
            color);

    ST7789_FillRect(
            x + 8,
            y + 2,
            1,
            5,
            color);

    ST7789_FillRect(
            x + 2,
            y + 8,
            5,
            1,
            color);
}


/* =========================================================
 * THERMOMETER ICON
 * ========================================================= */

static void UI_DrawThermometerIcon(
        int16_t x,
        int16_t y,
        uint16_t color)
{
    ST7789_FillRect(
            x + 4,
            y,
            2,
            12,
            color);

    ST7789_FillRect(
            x + 2,
            y + 10,
            6,
            2,
            color);

    ST7789_FillRect(
            x,
            y + 12,
            2,
            6,
            color);

    ST7789_FillRect(
            x + 8,
            y + 12,
            2,
            6,
            color);

    ST7789_FillRect(
            x + 2,
            y + 18,
            6,
            2,
            color);

    ST7789_FillRect(
            x + 4,
            y + 8,
            2,
            9,
            color);
}


/* =========================================================
 * STATUS LAMP
 * ========================================================= */

static void UI_DrawLamp(
        int16_t x,
        int16_t y,
        uint16_t color)
{
    ST7789_FillRect(
            x + 3,
            y,
            8,
            2,
            color);

    ST7789_FillRect(
            x,
            y + 2,
            14,
            8,
            color);

    ST7789_FillRect(
            x + 3,
            y + 10,
            8,
            2,
            color);
}


/* =========================================================
 * TEMPERATURE BAR
 * ========================================================= */

static void UI_DrawTemperatureBar(
        uint16_t temperature_x10)
{
    uint16_t level;


    /*
     * Nền bar
     */
    ST7789_FillRect(
            30,
            130,
            180,
            3,
            UI_DARK_GRAY);


    if (temperature_x10 >=
        TEMP_LIMIT_X10)
    {
        level = 180U;
    }
    else
    {
        level =
            (uint16_t)(
            ((uint32_t)temperature_x10 *
             180U)
            /
            TEMP_LIMIT_X10);
    }


    if (level > 0U)
    {
        ST7789_FillRect(
                30,
                130,
                (int16_t)level,
                3,
                UI_WHITE);
    }
}


/* =========================================================
 * MAKE TEMPERATURE TEXT
 *
 * Ví dụ:
 *
 * 28.5 -> " 28.5"
 * 105.3 -> "105.3"
 * ========================================================= */

static void UI_MakeTemperatureText(
        uint16_t temperature_x10,
        char *text)
{
    uint16_t integerPart;
    uint16_t decimalPart;


    integerPart =
            temperature_x10 /
            10U;


    decimalPart =
            temperature_x10 %
            10U;


    if (integerPart > 999U)
    {
        integerPart = 999U;

        decimalPart = 9U;
    }


    if (integerPart < 10U)
    {
        text[0] = ' ';
        text[1] = ' ';

        text[2] =
                (char)(
                '0' +
                integerPart);
    }
    else if (integerPart < 100U)
    {
        text[0] = ' ';

        text[1] =
                (char)(
                '0' +
                integerPart / 10U);

        text[2] =
                (char)(
                '0' +
                integerPart % 10U);
    }
    else
    {
        text[0] =
                (char)(
                '0' +
                integerPart / 100U);

        text[1] =
                (char)(
                '0' +
                (integerPart / 10U) %
                10U);

        text[2] =
                (char)(
                '0' +
                integerPart % 10U);
    }


    text[3] = '.';


    text[4] =
            (char)(
            '0' +
            decimalPart);


    text[5] = '\0';
}


/* =========================================================
 * DRAW TEMPERATURE NUMBER
 *
 * Nhiệt độ chỉ màu trắng
 * ========================================================= */

static void UI_DrawTemperatureNumber(
        uint16_t temperature_x10)
{
    char text[6];


    UI_MakeTemperatureText(
            temperature_x10,
            text);


    ST7789_DrawStringBG(
            24,
            72,
            text,
            UI_WHITE,
            UI_BLACK,
            5);
}


/* =========================================================
 * STATUS FRAME
 * ========================================================= */

static void UI_DrawStatusFrame(void)
{
    /*
     * Top
     */
    ST7789_FillRect(
            42,
            153,
            156,
            1,
            UI_DARK_GRAY);


    /*
     * Bottom
     */
    ST7789_FillRect(
            42,
            199,
            156,
            1,
            UI_DARK_GRAY);


    /*
     * Left
     */
    ST7789_FillRect(
            35,
            160,
            1,
            32,
            UI_DARK_GRAY);


    /*
     * Right
     */
    ST7789_FillRect(
            204,
            160,
            1,
            32,
            UI_DARK_GRAY);
}


/* =========================================================
 * CLEAR STATUS TEXT
 *
 * Chỉ xóa vùng chữ GOOD/WARNING
 * Không xóa cả khung
 * ========================================================= */

static void UI_ClearStatusText(void)
{
    ST7789_FillRect(
            78,
            165,
            120,
            28,
            UI_BLACK);
}


/* =========================================================
 * GOOD
 * ========================================================= */

static void UI_ShowGood(void)
{
    /*
     * Xóa WARNING cũ
     */
    UI_ClearStatusText();


    /*
     * Đèn trạng thái bình thường
     */
    UI_DrawLamp(
            52,
            170,
            UI_WHITE);


    /*
     * GOOD
     */
    ST7789_DrawString(
            82,
            169,
            "GOOD",
            UI_WHITE,
            3);


    /*
     * Text dưới
     */
    ST7789_DrawStringBG(
            48,
            214,
            "NORMAL LIMIT 105C ",
            UI_GRAY,
            UI_BLACK,
            1);
}


/* =========================================================
 * WARNING
 * ========================================================= */

static void UI_ShowWarning(void)
{
    uint8_t i;


    /*
     * Xóa GOOD cũ
     */
    UI_ClearStatusText();


    /*
     * Vẽ WARNING cố định
     */
    ST7789_DrawString(
            82,
            169,
            "WARNING",
            UI_RED,
            2);


    /*
     * Đèn nháy đỏ
     */
    for (i = 0U;
         i < 3U;
         i++)
    {
        UI_DrawLamp(
                52,
                170,
                UI_RED);


        UI_DelayShort();


        UI_DrawLamp(
                52,
                170,
                UI_DARK_RED);


        UI_DelayShort();
    }


    /*
     * Giữ đỏ
     */
    UI_DrawLamp(
            52,
            170,
            UI_RED);


    /*
     * Text dưới
     */
    ST7789_DrawStringBG(
            48,
            214,
            "OVER LIMIT 105C   ",
            UI_RED,
            UI_BLACK,
            1);
}


/* =========================================================
 * UPDATE STATUS
 * ========================================================= */

static void UI_UpdateStatus(
        uint16_t temperature_x10)
{
    uint8_t warning;


    if (temperature_x10 >
        TEMP_LIMIT_X10)
    {
        warning = 1U;
    }
    else
    {
        warning = 0U;
    }


    /*
     * Status không đổi
     * -> không redraw
     */
    if (warning ==
        displayed_warning)
    {
        return;
    }


    displayed_warning =
            warning;


    if (warning == 0U)
    {
        UI_ShowGood();
    }
    else
    {
        UI_ShowWarning();
    }
}


/* =========================================================
 * STATIC INTERFACE
 *
 * Chỉ vẽ một lần
 * ========================================================= */

static void UI_DrawStaticInterface(void)
{
    /*
     * Clear màn hình đúng 1 lần
     */
    ST7789_FillScreen(
            UI_BLACK);


    /* =====================================================
     * TOP ICON
     * ===================================================== */

    UI_DrawThermometerIcon(
            48,
            22,
            UI_GRAY);


    /* =====================================================
     * TITLE
     * ===================================================== */

    ST7789_DrawString(
            76,
            26,
            "COOLANT",
            UI_GRAY,
            2);


    /* =====================================================
     * DEGREE C
     *
     * Vẽ 1 lần
     * Không đổi theo nhiệt độ
     * ===================================================== */

    UI_DrawDegree(
            181,
            83,
            UI_WHITE);


    ST7789_DrawString(
            195,
            81,
            "C",
            UI_WHITE,
            3);


    /* =====================================================
     * SEPARATOR
     * ===================================================== */

    ST7789_FillRect(
            20,
            143,
            200,
            1,
            UI_DARK_GRAY);


    /* =====================================================
     * STATUS FRAME
     * ===================================================== */

    UI_DrawStatusFrame();
}


/* =========================================================
 * UI INIT
 * ========================================================= */

void UI_Init(void)
{
    ST7789_Init();


    UI_DrawStaticInterface();


    displayed_temperature_x10 =
            0xFFFFU;


    displayed_warning =
            0xFFU;
}


/* =========================================================
 * PUBLIC UPDATE
 *
 * main chỉ gọi hàm này
 * ========================================================= */

void UI_UpdateTemperature(
        uint16_t temperature_x10)
{
    /*
     * Nếu nhiệt độ hiển thị chưa đổi 0.1C
     * thì không vẽ lại
     */
    if (temperature_x10 ==
        displayed_temperature_x10)
    {
        return;
    }


    /* =====================================================
     * NUMBER
     * ===================================================== */

    UI_DrawTemperatureNumber(
            temperature_x10);


    /* =====================================================
     * BAR
     * ===================================================== */

    UI_DrawTemperatureBar(
            temperature_x10);


    /* =====================================================
     * STATUS
     * ===================================================== */

    UI_UpdateStatus(
            temperature_x10);


    displayed_temperature_x10 =
            temperature_x10;
}
