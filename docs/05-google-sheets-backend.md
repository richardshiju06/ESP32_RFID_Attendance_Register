# Google Sheets Backend

The file [`../code/appscript_code_for_google_sheets.gs`](../code/appscript_code_for_google_sheets.gs) is the backend used by the ESP32.

## Spreadsheet structure

Create a Google Spreadsheet with these two sheet names exactly:

```text
User_Data
Attendance
```

### User_Data

Use:

| Column | Data |
|---|---|
| A | User name |
| B | RFID UID |

### Attendance

Use:

| Column | Data |
|---|---|
| A | User name |
| B | RFID UID |
| C | Date |
| D | Time In |
| E | Time Out |

## How the backend responds

The ESP32 expects a response beginning with `OK` and then a comma-separated status.

Common statuses are:

| Status | Meaning |
|---|---|
| `R_Successful` | New UID registered |
| `regErr01` | UID is already registered |
| `TI_Successful` | Time In recorded |
| `TO_Successful` | Time Out recorded |
| `atcErr01` | UID is not registered |
| `atcInf01` | Today's Time In and Time Out are already complete |

## Time zone

The Apps Script is configured to use `Asia/Kolkata` and formats the attendance date and time accordingly.

## Apps Script deployment

1. Open the Google Sheet.
2. Go to **Extensions → Apps Script**.
3. Copy the contents of `code/google_apps_script.gs` into the editor.
4. Replace `YOUR_GOOGLE_SHEET_ID` with your spreadsheet ID.
5. Deploy the project as a Web App.
6. Set access according to your intended deployment.
7. Copy the Web App URL.
8. Put that URL into the ESP32 sketch locally.
