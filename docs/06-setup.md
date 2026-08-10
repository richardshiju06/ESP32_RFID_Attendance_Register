# Setup Guide

## 1. Build the circuit

Connect the ESP32, MFRC522, LCD and push button according to [`03-wiring.md`](03-wiring.md).

## 2. Prepare Arduino IDE

Install the ESP32 board support package and select your ESP32 board.

Install these libraries:

- MFRC522
- LiquidCrystal_I2C

## 3. Prepare Google Sheets

Create a spreadsheet with `User_Data` and `Attendance` sheets. Follow [`05-google-sheets-backend.md`](05-google-sheets-backend.md).

## 4. Deploy the backend

Open Apps Script, add `code/google_apps_script.gs`, set the Sheet ID and deploy it as a Web App.

## 5. Configure the ESP32

Open `code/esp32_rfid_attendance.ino` and change these three values:

```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
String Web_App_URL = "YOUR_GOOGLE_APPS_SCRIPT_WEB_APP_URL";
```

Do this locally. Do not commit your real password or private URL.

## 6. Upload

Connect the ESP32 to the computer and upload the sketch.

Open Serial Monitor at **115200 baud** if you want to see connection and HTTP debugging messages.

## 7. Register a card

Press the button until the LCD shows Registration mode. Tap an RFID card/keychain. A new UID should be added to `User_Data`.

## 8. Test attendance

Switch to Attendance mode and scan the registered card. The first scan records Time In. Scan the same card again to record Time Out.

## 9. Verify the sheet

Check the `Attendance` sheet after each scan.
