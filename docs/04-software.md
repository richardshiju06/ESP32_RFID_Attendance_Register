# Software and Firmware

The ESP32 firmware is in [`../code/esp32_rfid_attendance.ino`](../code/esp32_rfid_attendance.ino).

## Libraries used

```cpp
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include "WiFi.h"
#include <HTTPClient.h>
```

`SPI`, `WiFi` and `HTTPClient` come with the ESP32 Arduino core. The other libraries need to be installed through Arduino IDE's Library Manager.

## Firmware flow

At startup the ESP32 initializes the LCD, SPI interface and RFID reader, then connects to Wi-Fi. If Wi-Fi does not connect within the configured timeout, the board restarts.

The main loop checks the button, reads an RFID card and then acts according to the current mode.

## Attendance request

The ESP32 sends the RFID UID to the Apps Script using:

```text
?sts=atc&uid=<UID>
```

## Registration request

For registration it sends:

```text
?sts=reg&uid=<UID>
```

## LCD messages

The firmware provides feedback such as:

- `UID Read OK`
- `UID registered successfully`
- `Error! UID already exists`
- `Error! Card not registered`
- `Attendance done for today`
- `WiFi Error! Disconnected`
