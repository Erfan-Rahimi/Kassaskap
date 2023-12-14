/*
File: my_program.cpp
Author: Erfan Rahimi
Date: 2023-12-20
Descriotion: This program uses the two components, RFID and IR-reciever in order to activate and rotate a Servo. It is essentially a lock that uses
a card system with the RFID so that only the owner can access what ever they are trying to protect with the lock or a password set with a remote controller 
. The owner can also change the password of the remote controller at anytime to any three digit number they want, and the remote controller comunicates with
the IR-reciever and uses Infra red signals in order to send different codes, which allows us to choose a pattern.

*/

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

/* Includes the necessary libraries for IR communication, Servo library for controlling the servo motor
SPI library for communication with RFID module, MFRC522 for working with RFID cards,
Wire library for 12C communication with OLED display, U8glib library for handling the OLED display.
*/

#include <IRremote.h>
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <U8glib.h>


// Defines reset pin for RFID module 
#define RST_PIN 9
// Define the slave select pin for the RFID module
#define SS_PIN 10
// Defines the pin that IR reciever is connected to...
const int IR_PIN = 5;

// Initilizes OLED display object with 12C communication
U8GLIB_SSD1306_128X64 oled(U8G_I2C_OPT_NO_ACK);

// Creates a Servo object 
Servo myServo;

MFRC522 mfrc522(SS_PIN, RST_PIN); // Defines RFID module pins 
String MasterTag = "A9188BD";
String tagID = "";
boolean getID();
bool closed = true; // Variable to track the state of the locker

// Defining the IR password
unsigned long pattern[3] = { 0x16, 0x19, 0xD };
int patternIndex = 0; // Variable that tracks the current button press in the array
bool changingPassword = false;
// Define the IR receiver object
IRrecv irrecv(IR_PIN);


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

/* 
   Description: Updates the current text to the oled
   Parameters:  The text that is going to be displayed
   Returns:     
*/

void updateOled(String text){
    oled.firstPage();  // Starts the initial page of the OLED
  do{
    oled.drawStr(0,11, text.c_str());  // Draws or writes the text on the OLED with given coordinates
  } while (oled.nextPage());  // Moves to the next page
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

/* 
   Description: This does the password change process 
   Parameters:  The new given indexes?
   Returns:     The new password
*/

void changePassword() {
  updateOled("Please set a new password");  // Tempts the user to change the password
  unsigned long newPattern[3] = { 0, 0, 0 };  // Creates an array to store the password
  int newPatternIndex = 0; // Creates index that tracks the new password elements

// Loop that catches the signals from the IR remote 
  while (newPatternIndex < 3) {
    if (irrecv.decode()) {
      updateOled(String(irrecv.decodedIRData.command, HEX)); // Displays the IR codes on the OLED
      delay(300);
      newPattern[newPatternIndex] = irrecv.decodedIRData.command; //Stores the IR codes in the array
      newPatternIndex++;
      irrecv.resume();  // Receive the next value
    }
  }

// This copies the new password to the main password array
  for (int i = 0; i < 3; i++) {
    pattern[i] = newPattern[i];
  }

  updateOled("Password changed successfully!"); // Comes up when the password changes successfully
  changingPassword = false; // Sets the flag that indicates the successfull password change
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

 /* 
   Description: Reads the RFID tag and checks its ID
   Parameters:  None
   Returns:     boolean - true if there is a valid RFID tag, false if not
*/

boolean getID() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return false; // Exists function if there is no new card
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return false; // Can not read card, exit function
  }
  tagID = ""; // Starts the tagID variabel that stores the tags ID
  // Loop that converts each byte of the RFID UID to hexadecimal
  for (uint8_t i = 0; i < 4; i++) {
    tagID.concat(String(mfrc522.uid.uidByte[i], HEX));
    delay(300);
  }
  tagID.toUpperCase(); // Converts the variabel ID to uppercase
  mfrc522.PICC_HaltA();  // Stop reading
  updateOled(tagID); // Uptade oled with given ID
  return true; // Return true to indicate a successful RFID tag read
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

 /* 
   Description: Checks IR input and validates the recieved signals
   Parameters:  None
   Returns:     void
*/

void checkIR() {
  if (irrecv.decode()) {
    delay(300);
    updateOled(String(irrecv.decodedIRData.command, HEX)); // Displays the IR code on OLED
    delay(300);

    // Checks if the recieved IR code matches the command for password change
    if (irrecv.decodedIRData.decodedRawData == 0xB54AFF00) {
      changingPassword = true; // Sets the flag that intinates password change
    } else {
      // Checks if the Recieved IR code matches the pattern
      if (irrecv.decodedIRData.command == pattern[patternIndex]) {
        patternIndex++; // Moves to the next element
        // Checks if the whole pattern has been entered
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

    // Controls servo depending on the value of closed
    if (closed) {
      myServo.write(0);
    } else {
      myServo.write(200);
    }

    irrecv.resume();  // Receive the next value
  }
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

 /* 
   Description: Checks if the RFID matches the Mastertag
   Parameters:  None
   Returns:     void
*/

void checkRFID() {
  // Checks for RFID input as long as there is a valid RFID tag present
  while (getID()) {
    // Checks if the userTag matches the Mastertag
    if (tagID == MasterTag) {
      updateOled("Access Granted");
      closed = !closed;
    } else {
      updateOled("Access denied");
    }

    // Control the servo based on the state of 'closed
    if (closed) {
      myServo.write(0);
    } else {
      myServo.write(200);
    }
  }
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

void setup() {
  Serial.begin(9600); // Starts serial communication
  Wire.begin(); // Start the 12C communcation for OLED
  oled.setFont(u8g_font_helvB10); // Sets font for OLED
  SPI.begin(); // Starts SPI communication for RFID
  mfrc522.PCD_Init(); // Starts the RDID module
  delay(4); // Puts delay for RFID module stability
  mfrc522.PCD_DumpVersionToSerial(); // Dump the RFID module version information to serial
  myServo.attach(8); // Servo attached to 8
  myServo.write(60); // Moves servo to the initial position
  irrecv.enableIRIn(); // Enables th IR reciever for remote input
  updateOled("Hello! Please enter your password");
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

 /* 
   Description: Checks RFID access.
   Parameters:  None
   Returns:     void
*/

void loop() {
  // Check if the system is in the process of changing the password
  if (changingPassword) {
    changePassword(); // If changing password, then execute the password change procedure
  } else {
    checkIR(); // Calls on the checkIR function that starts to check for IR signals 
    checkRFID(); // Calls on the RFID function that starts to check for the RFID signals.
  }
}

