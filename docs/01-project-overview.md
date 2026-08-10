# Project Overview

## Why this project?

Taking attendance manually works, but it becomes repetitive very quickly. This project replaces that routine with a small RFID-based system that can identify a person and send the attendance information to Google Sheets.

The ESP32 handles the hardware side. The MFRC522 reads the RFID card, the LCD tells the user what is happening, and Google Apps Script handles the communication with the spreadsheet.

## Main blocks

```text
RFID Card
   ↓
MFRC522
   ↓ SPI
ESP32
   ├── I2C → 16×2 LCD
   ├── GPIO → Mode button
   └── Wi-Fi → Google Apps Script → Google Sheets
```

## Two modes

### Attendance Mode

Used for normal attendance. A registered card can be used to record Time In and then Time Out.

### Registration Mode

Used when a new RFID card needs to be added. The UID is checked first so the same card is not registered again.

## Backend sheets

The project uses two sheets:

- `User_Data` — stores the user name and RFID UID.
- `Attendance` — stores the daily attendance information.

The Apps Script receives two main request types: `reg` for registration and `atc` for attendance.
