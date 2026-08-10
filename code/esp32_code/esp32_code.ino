#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include "WiFi.h"
#include <HTTPClient.h>

// RFID pins
#define SS_PIN  5  
#define RST_PIN 4

// Button pin
#define BTN_PIN 15

// LCD size for 16x2
int lcdColumns = 16;
int lcdRows = 2;

// WiFi config
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Google script Web_App_URL.
String Web_App_URL = "YOUR_GOOGLE_APPS_SCRIPT_WEB_APP_URL";

// App variables
String reg_Info = "";
String atc_Info = "";
String atc_Name = "";
String atc_Date = "";
String atc_Time_In = "";
String atc_Time_Out = "";

// Variable to read data from RFID-RC522.
int readsuccess;
char str[32] = "";
String UID_Result = "--------";
String modes = "atc";

// Create LCD for 16x2 display
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

// Create MFRC522 object
MFRC522 mfrc522(SS_PIN, RST_PIN);

// ---- Utility functions ----
String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void byteArray_to_string(byte array[], unsigned int len, char buffer[]) {
  for (unsigned int i = 0; i < len; i++) {
    byte nib1 = (array[i] >> 4) & 0x0F;
    byte nib2 = (array[i] >> 0) & 0x0F;
    buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
    buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
  }
  buffer[len*2] = '\0';
}

int getUID() {  
  if(!mfrc522.PICC_IsNewCardPresent()) return 0;
  if(!mfrc522.PICC_ReadCardSerial()) return 0;
  byteArray_to_string(mfrc522.uid.uidByte, mfrc522.uid.size, str);
  UID_Result = str;
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  return 1;
}

// ---- HTTP Request to Google Sheets ----
void http_Req(String str_modes, String str_uid) {
  if (WiFi.status() == WL_CONNECTED) {
    String http_req_url = "";
    if (str_modes == "atc") {
      http_req_url  = Web_App_URL + "?sts=atc";
      http_req_url += "&uid=" + str_uid;
    }
    if (str_modes == "reg") {
      http_req_url = Web_App_URL + "?sts=reg";
      http_req_url += "&uid=" + str_uid;
    }
    Serial.println();
    Serial.println("Sending request to Google Sheets...");
    Serial.print("URL : ");
    Serial.println(http_req_url);

    HTTPClient http;
    http.begin(http_req_url.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();
    Serial.print("HTTP Status Code : ");
    Serial.println(httpCode);

    String payload;
    if (httpCode > 0) {
      payload = http.getString();
      Serial.println("Payload : " + payload);  
    }
    http.end();

    String sts_Res = getValue(payload, ',', 0);

    //--- LCD Display for 16x2 ---
    if (sts_Res == "OK") {
      if (str_modes == "atc") {
        atc_Info = getValue(payload, ',', 1);
        if (atc_Info == "TI_Successful") {
          atc_Name = getValue(payload, ',', 2);
          atc_Date = getValue(payload, ',', 3);
          atc_Time_In = getValue(payload, ',', 4);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Name:");
          lcd.print(atc_Name.substring(0, lcdColumns-5));
          lcd.setCursor(0,1);
          lcd.print(atc_Date + " " + atc_Time_In);
          delay(2000);
          lcd.clear();
        }
        if (atc_Info == "TO_Successful") {
          atc_Name = getValue(payload, ',', 2);
          atc_Date = getValue(payload, ',', 3);
          atc_Time_In = getValue(payload, ',', 4);
          atc_Time_Out = getValue(payload, ',', 5);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Name:");
          lcd.print(atc_Name.substring(0, lcdColumns-5));
          lcd.setCursor(0,1);
          lcd.print(atc_Time_In + "-" + atc_Time_Out);
          delay(2000);
          lcd.clear();
        }
        if (atc_Info == "atcInf01") {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Attendance done");
          lcd.setCursor(0,1);
          lcd.print("for today");
          delay(2000);
          lcd.clear();
        }
        if (atc_Info == "atcErr01") {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Error! Card");
          lcd.setCursor(0,1);
          lcd.print("not registered");
          delay(2000);
          lcd.clear();
        }
        atc_Info = ""; atc_Name = ""; atc_Date = ""; atc_Time_In = ""; atc_Time_Out = "";
      }
      if (str_modes == "reg") {
        reg_Info = getValue(payload, ',', 1);
        if (reg_Info == "R_Successful") {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("UID registered");
          lcd.setCursor(0,1);
          lcd.print("successfully");
          delay(2000);
          lcd.clear();
        }
        if (reg_Info == "regErr01") {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Error! UID");
          lcd.setCursor(0,1);
          lcd.print("already exists");
          delay(2000);
          lcd.clear();
        }
        reg_Info = "";
      }
    }
  } else {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("WiFi Error!");
    lcd.setCursor(0,1);
    lcd.print("Disconnected");
    delay(2000);
    lcd.clear();
  }
}

// ---- Setup ----
void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(BTN_PIN, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  SPI.begin();
  mfrc522.PCD_Init();
  delay(500);

  lcd.setCursor(0,0);
  lcd.print("ESP32 RFID");
  lcd.setCursor(0,1);
  lcd.print("Attendance");
  delay(1500);
  lcd.clear();

  Serial.println("WIFI mode : STA");
  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int connecting_process_timed_out = 40; // 20 seconds (40 x 0.5s)
  while (WiFi.status() != WL_CONNECTED) {
    lcd.setCursor(0,0);
    lcd.print("Connecting WiFi");
    delay(500);
    lcd.clear();
    connecting_process_timed_out--;
    if (connecting_process_timed_out == 0) {
      delay(1000);
      ESP.restart();
    }
  }
  lcd.setCursor(0,0);
  lcd.print("WiFi Connected");
  delay(1500);
  lcd.clear();
}

// ---- Loop ----
void loop() {
  int BTN_State = digitalRead(BTN_PIN);
  if (BTN_State == LOW) {
    if (modes == "atc") {
      modes = "reg";
    } else if (modes == "reg") {
      modes = "atc";
    }
    delay(500);
  }

  readsuccess = getUID();

  if (modes == "atc") {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("ATTENDANCE");
    lcd.setCursor(0,1);
    lcd.print("Tap card/keychain");
    if (readsuccess) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("UID Read OK");
      lcd.setCursor(0,1);
      lcd.print("Please wait...");
      delay(1000);
      http_Req(modes, UID_Result);
    }
  }

  if (modes == "reg") {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("REGISTRATION");
    lcd.setCursor(0,1);
    lcd.print("Tap card/keychain");
    if (readsuccess) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("UID Read OK");
      lcd.setCursor(0,1);
      lcd.print("UID: ");
      lcd.print(UID_Result.substring(0, lcdColumns-5));
      delay(1000);
      http_Req(modes, UID_Result);
    }
  }

  delay(10);
}
