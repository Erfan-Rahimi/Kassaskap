#include <IRremote.h>
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <U8glib.h>


#define RST_PIN 9
#define SS_PIN 10
const int IR_PIN = 5;

// Define the IR receiver object
IRrecv irrecv(IR_PIN);

U8GLIB_SSD1306_128X64 oled(U8G_I2C_OPT_NO_ACK);

Servo myServo;

// for RFID
MFRC522 mfrc522(SS_PIN, RST_PIN);
String MasterTag = "A9188BD";
String tagID = "";
boolean getID();
bool closed = true;

// Define the pattern or password
unsigned long pattern[3] = { 0x16, 0x19, 0xD };
int patternIndex = 0;
bool changingPassword = false;

void updateOled(String text){
    oled.firstPage();
  do{
    oled.drawStr(0,11, text.c_str());
  } while (oled.nextPage());
}

void changePassword() {
  updateOled("Please set a new password");
  unsigned long newPattern[3] = { 0, 0, 0 };
  int newPatternIndex = 0;

  while (newPatternIndex < 3) {
    if (irrecv.decode()) {
      updateOled(String(irrecv.decodedIRData.command, HEX));
      delay(300);
      newPattern[newPatternIndex] = irrecv.decodedIRData.command;
      newPatternIndex++;
      irrecv.resume();  // Receive the next value
    }
  }

  for (int i = 0; i < 3; i++) {
    pattern[i] = newPattern[i];
  }

  updateOled("Password changed successfully!");
  changingPassword = false;
}

boolean getID() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return false;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return false;
  }
  tagID = "";
  for (uint8_t i = 0; i < 4; i++) {
    tagID.concat(String(mfrc522.uid.uidByte[i], HEX));
    delay(300);
  }
  tagID.toUpperCase();
  mfrc522.PICC_HaltA();  // Stop reading
  updateOled(tagID);
  return true;
}

void checkIR() {
  if (irrecv.decode()) {
    delay(300);
    updateOled(String(irrecv.decodedIRData.command, HEX));
    delay(300);
    if (irrecv.decodedIRData.decodedRawData == 0xB54AFF00) {
      changingPassword = true;
    } else {
      if (irrecv.decodedIRData.command == pattern[patternIndex]) {
        patternIndex++;
        if (patternIndex >= sizeof(pattern) / sizeof(pattern[0])) {
          updateOled("Access granted");
          patternIndex = 0;  // Reset pattern index after successful access
          closed = !closed;
        }
      } else {
        patternIndex = 0;  // Reset if incorrect code
        updateOled("Access denied");
      }
    }

    if (closed) {
      myServo.write(0);
    } else {
      myServo.write(200);
    }

    irrecv.resume();  // Receive the next value
  }
}

void checkRFID() {
  while (getID()) {
    if (tagID == MasterTag) {
      updateOled("Access Granted");
      closed = !closed;
    } else {
      updateOled("Access denied");
    }

    if (closed) {
      myServo.write(0);
    } else {
      myServo.write(200);
    }
  }
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  oled.setFont(u8g_font_helvB10);
  SPI.begin();
  mfrc522.PCD_Init();
  delay(4);
  mfrc522.PCD_DumpVersionToSerial();
  myServo.attach(8);
  myServo.write(60);
  irrecv.enableIRIn();
  updateOled("Hello! Please enter your password");
}

void loop() {
  if (changingPassword) {
    changePassword();
  } else {
    checkIR();
    checkRFID();
  }
}

