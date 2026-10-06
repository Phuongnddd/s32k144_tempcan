S32K144 LM35 + CAN + ST7789 Project
1. Tổng quan
Project sử dụng 2 board S32K144:
- Board gửi (TX):
  - Đọc nhiệt độ từ cảm biến LM35 bằng ADC.
  - Chuyển nhiệt độ sang dạng x100.
  - Gửi nhiệt độ qua CAN với ID 0x123.
  - Sử dụng transceiver TJA1050.
- Board nhận (RX):
  - Nhận dữ liệu CAN từ board gửi.
  - Ghép 2 byte nhiệt độ.
  - Hiển thị nhiệt độ lên màn hình ST7789.
  - Giao diện có 2 trạng thái:
    - GOOD
    - WARNING khi vượt ngưỡng.
2. Luồng hoạt động
LM35
  |
  v
ADC - S32K144 TX
  |
  v
CAN0 TX/RX
  |
  v
TJA1050
  |
  | CANH / CANL
  v
TJA1050
  |
  v
CAN0 - S32K144 RX
  |
  v
ST7789
3. Kết nối LM35 với board gửi
LM35	S32K144
VCC	5V
GND	GND
VOUT	PTC3 / ADC0 channel 11


Trong code:
#define ADC_CHANNEL_PTC3 11U
LM35 có hệ số:
10 mV / °C
Ví dụ:
25 °C -> khoảng 0.25 V
30 °C -> khoảng 0.30 V
4. Kết nối TJA1050 với S32K144
Áp dụng cho cả board gửi và board nhận.
TJA1050	Kết nối
Pin 1 - TXD	PTE5 / CAN0_TX
Pin 2 - GND	GND
Pin 3 - VCC	5V
Pin 4 - RXD	PTE4 / CAN0_RX
Pin 5 - VREF	Không nối
Pin 6 - CANL	CANL của bus CAN
Pin 7 - CANH	CANH của bus CAN
Pin 8 - S	GND


Kết nối giữa hai transceiver
TJA1050 TX                     TJA1050 RX

CANH  ----------------------  CANH

CANL  ----------------------  CANL

GND   ----------------------  GND
Hai board phải chung GND.
Nếu hai board là hai đầu cuối của bus CAN, nên mắc điện trở:
120 ohm giữa CANH và CANL
ở mỗi đầu bus.
5. Kết nối ST7789 với board nhận
ST7789	S32K144
SCL / SCK	PTB2 - LPSPI0_SCK
SDA / MOSI	PTB4 - LPSPI0_SOUT
CS	PTB5
DC	PTC12
RST / RES	PTC13
GND	GND
VCC	Theo module ST7789 đang sử dụng


Trong driver hiện tại:
#define LCD_CS_PIN   5U
#define LCD_DC_PIN   12U
#define LCD_RST_PIN  13U
6. Cấu hình CAN
Project gửi dữ liệu với:
CAN ID = 0x123
DLC    = 2 byte
Nhiệt độ được gửi theo dạng:
28.53 °C -> 2853
Tách thành 2 byte:
canData[0] = (uint8_t)(temperature_x100 >> 8);
canData[1] = (uint8_t)(temperature_x100 & 0xFFU);
Board nhận ghép lại:
rx_temperature_x100 =
        ((uint16_t)data[0] << 8)
        |
        ((uint16_t)data[1]);
7. Công thức ADC và LM35
ADC sử dụng độ phân giải 12-bit:
0 -> 4095
Điện áp tham chiếu:
#define ADC_VREF        3.3f
#define ADC_MAX_VALUE   4095.0f
Công thức đổi ADC sang điện áp:
Voltage = ADC_raw × 3.3 / 4095
Công thức LM35:
Temperature = Voltage × 100
Ví dụ:
ADC raw ≈ 310

Voltage ≈ 310 × 3.3 / 4095
        ≈ 0.25 V

Temperature ≈ 0.25 × 100
            ≈ 25 °C
8. Các biến debug ADC
Có thể thêm vào Expressions trong S32 Design Studio:
adc_debug_raw
adc_debug_voltage
lm35_debug_temperature
lm35_debug_temperature_x100
Ý nghĩa:
Biến	Ý nghĩa
adc_debug_raw	Giá trị ADC raw
adc_debug_voltage	Điện áp tính được
lm35_debug_temperature	Nhiệt độ dạng float
lm35_debug_temperature_x100	Nhiệt độ nhân 100


Ví dụ ở khoảng 25 °C:
adc_debug_raw                 ≈ 310
adc_debug_voltage             ≈ 0.250
lm35_debug_temperature        ≈ 25.0
lm35_debug_temperature_x100   ≈ 2500
9. Các biến debug CAN TX
temperature
temperature_x100
tx_byte0
tx_byte1
tx_status
send_count
Ví dụ:
temperature       = 28.50
temperature_x100  = 2850
10. Các biến debug CAN RX
rx_id
rx_length
rx_data0
rx_data1
rx_temperature_x100
rx_temperature
receive_count
11. Giao diện ST7789
Giao diện hiển thị:
        COOLANT

        28.5 °C

------------------------

       [ GOOD ]

   NORMAL LIMIT 105C
Khi vượt ngưỡng:
        COOLANT

       108.2 °C

------------------------

      [ WARNING ]

    OVER LIMIT 105C
Ngưỡng cảnh báo:
#define TEMP_LIMIT_X10 1050U
Tương ứng:
105.0 °C
12. Trạng thái hiển thị
Chỉ có 2 trạng thái:
Temperature <= 105.0 °C
-> GOOD
Temperature > 105.0 °C
-> WARNING
Khi chuyển sang WARNING:
- Chữ WARNING được vẽ màu đỏ.
- Đèn cảnh báo nháy.
- Sau khi nháy xong, đèn giữ màu đỏ.
Khi quay lại dưới ngưỡng:
- Vùng chữ trạng thái cũ được xóa.
- Hiển thị lại GOOD.
13. Cấu trúc project đề xuất
src/
|
|-- main.c
|-- adc.c
|-- adc.h
|-- can.c
|-- can.h
|-- spi.c
|-- spi.h
|-- st7789.c
|-- st7789.h
|-- ui.c
|-- ui.h
Nhiệm vụ từng file
adc.c / adc.h
-> ADC + LM35

can.c / can.h
-> CAN driver

spi.c / spi.h
-> SPI driver

st7789.c / st7789.h
-> driver màn hình ST7789 + font

ui.c / ui.h
-> giao diện nhiệt độ + GOOD/WARNING

main.c
-> gọi driver, nhận/gửi dữ liệu
14. Sơ đồ dây tổng thể
               BOARD GỬI
             S32K144 TX

LM35
VCC  ---------------- 5V
GND  ---------------- GND
VOUT ---------------- PTC3

                  |
                  | ADC
                  v

            S32K144 TX
            PTE5 CAN_TX
            PTE4 CAN_RX
                  |
                  v
               TJA1050

CANH ================================= CANH
CANL ================================= CANL
GND  ================================= GND

               TJA1050
                  |
                  v
            S32K144 RX
            PTE5 CAN_TX
            PTE4 CAN_RX

                  |
                  v

                ST7789

PTB2  ---------------- SCK
PTB4  ---------------- MOSI
PTB5  ---------------- CS
PTC12 ---------------- DC
PTC13 ---------------- RST
GND   ---------------- GND
