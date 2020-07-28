/*------------------------------------------------------------------------------
  PROJECT:      Module tests
  FILE:         comboI2C_lcd_rfid.ino
  AUTHOR:       SSQ66648
  VERSION:      v1.0
  DESCRIPTION:  Testing and experimentation of Arduino module cobination: RFID-RC522 and LCD-1602A
  ------------------------------------------------------------------------------
  NOTES:
    +
  TESTING RESULTS:
    v1.0:
      Copied releveant parts of LCD test code: still functional.
      setting lcd.begin(0,0) resulted in 2*missing characters from serial input on line 1: returned to begin(16,1).
      did not need to tackle I2C adresses as neither module required same pins.
      current version of comparing UID iteratively takes FAR too long: to be addressed in v2.0
      built in led not outputting high: will test if working on own sketch.
  TODO:
    + Implement faster comparison of UIDs
  (lesser) TODO:
    + test LED_BUILTIN is working/fix
    + add LEDs for "access granted" or denied (not entirely relevant but more interesting and relays information at a glance)
  ----------------------------------------------------------------------------*/

//Header files------------------------------------------------------------------
//lcd module
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//rfid module
#include <SPI.h>
#include <MFRC522.h>


//Definitions-------------------------------------------------------------------
#define RST_PIN         9           // RFID rst pin
#define SS_PIN          10          // RFID ss pin


//Constructors------------------------------------------------------------------
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create rfid instance.


//Member variables--------------------------------------------------------------
int targetCode[] = {89, 213, 190, 178}; //"authenticed UUID" (testing white Card UID (hex): 59 D5 BE B2)


//Setup-------------------------------------------------------------------------
void setup() {
  //feedback LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(9600);
  //Specify which program is currently running on Arduino
  Serial.println(F("START " __FILE__ " from " __DATE__ "\r\n"));

  //-----initialise LCD steps------
  lcd.init();
  lcd.backlight();

  //testing: previously (16,1)
  lcd.begin(16, 1);

  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("RFID test...");
  delay(1000);
  lcd.clear();
  lcd.print("Scan Card: ");

  //-----initialise RFID steps-----
  while (!Serial);    // Do nothing if no serial port is opened
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card

  Serial.println("Setup complete");
  Serial.println();
}



//Loop--------------------------------------------------------------------------
void loop() {
  //reverse of the "if - no - card / read - then - exit" loops: may change back to that version
  if ( mfrc522.PICC_IsNewCardPresent()) {
    Serial.println("debug: card found");
    if ( mfrc522.PICC_ReadCardSerial()) {
      Serial.println("debug: reading serial...");
      Serial.println();

      //print scanned ID number to both LCD and serial monitor
      Serial.print("Scanned UUID: ");
      lcd.clear();
      //lcd.setCursor(0, 0);
      lcd.print("Scanned UUID: ");
      lcd.setCursor(0, 1);

      //print each byte as hex values (serial and lcd)
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);

        lcd.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        lcd.print(mfrc522.uid.uidByte[i], HEX);
      }
      Serial.println();

      //leave on screen for a moment
      delay(3000);

      //check found UID against expected one:
      int i = 0;
      boolean match = true;
      //iterate bytes and disable if mismatch found
      while (i < mfrc522.uid.size) {
        if (!(int(mfrc522.uid.uidByte[i]) == int(targetCode[i]))) {
          match = false;
          //show when mismatch occurs on LED
          digitalWrite(LED_BUILTIN, HIGH);
        }
        i++;
      }

      delay(3000);
      lcd.clear();
      lcd.setCursor(0, 0);

      if (match) {
        lcd.print("UUID: MATCH");
        Serial.println("Card MATCHED");
      } else {
        lcd.print("UUID: DENIED");
        Serial.println("Card NOT matched");
      }
      Serial.println(" == == == == == == == == == == == == == == ");
      //end read
      mfrc522.PICC_HaltA();

      //leave on screen for a moment
      delay(3000);

      //reset
      lcd.clear();
      lcd.print("Scan Card");
      digitalWrite(LED_BUILTIN, LOW);

    }
  }

  //  if (Serial.available()) {
  //    Serial.println("debug: serial input");
  //
  //    //delay to allow entire message to arrive
  //    delay(100);
  //    //remove all previous display
  //    lcd.clear();
  //    //redisplay instruction line and move cursor to second line for input display
  //    lcd.print("Serial input: ");
  //    lcd.setCursor(0, 1);
  //
  //    char inchar;
  //    while (Serial.available() > 0) {
  //      //input character from incoming serial
  //      inchar = Serial.read();
  //      //restrict input characters to text only (remove issues such as newline charater from attempting to print)
  //      if ((' ' <= inchar) && (inchar <= '~'))
  //        //build string to print
  //        inputString += inchar;
  //    }
  //    lcd.print(inputString);
  //  }

}


//Functions---------------------------------------------------------------------
