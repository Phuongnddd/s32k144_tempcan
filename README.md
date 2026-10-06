

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
