# Troubleshooting

## RFID card is not detected

- Check the MFRC522 wiring.
- Make sure the reader has the correct 3.3 V supply.
- Check the SPI pins.
- Confirm that the card is close enough to the reader.

## LCD stays blank

- Check VCC and GND.
- Check SDA → GPIO 21 and SCL → GPIO 22.
- Confirm the LCD I2C address is `0x27`.

## ESP32 cannot connect to Wi-Fi

- Check the SSID and password.
- Make sure the network is available to the ESP32.
- Remember that the current firmware restarts after the connection timeout.

## Google Sheets is not updating

- Check the Web App URL.
- Make sure the Apps Script is deployed correctly.
- Check the Google Sheet ID in the Apps Script.
- Confirm the sheet names are exactly `User_Data` and `Attendance`.
- Check Serial Monitor for the HTTP status code and returned payload.

## Card says 'not registered'

The UID is not present in `User_Data`. Switch to Registration mode and register the card first.

## UID already exists

The card has already been registered. Use that card in Attendance mode instead of registering it again.
