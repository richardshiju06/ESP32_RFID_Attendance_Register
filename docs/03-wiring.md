# Wiring

The pin assignments below match the current ESP32 sketch.

## MFRC522 → ESP32

| MFRC522 | ESP32 |
|---|---|
| SDA / SS | GPIO 5 |
| RST | GPIO 4 |
| SCK | GPIO 18 |
| MOSI | GPIO 23 |
| MISO | GPIO 19 |
| 3.3V | 3.3V |
| GND | GND |

The RFID reader uses the ESP32 SPI interface.

## 16×2 I2C LCD → ESP32

| LCD | ESP32 |
|---|---|
| SDA | GPIO 21 |
| SCL | GPIO 22 |
| VCC | 5V |
| GND | GND |

The sketch uses I2C address `0x27`.

## Push button → ESP32

| Button | ESP32 |
|---|---|
| One side | GPIO 15 |
| Other side | GND |

The button uses the ESP32 internal pull-up (`INPUT_PULLUP`), so a pressed button reads LOW.

## Circuit diagram

![Circuit diagram](../images/circuit_diagram.png)
