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
  TODO:
    + connect both modules to board
    + find out if module addresses need to be changed to do so (ie both use Serial?)
  (lesser) TODO:
    +
  ----------------------------------------------------------------------------*/

//Header files------------------------------------------------------------------
//lcd module
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


//Definitions-------------------------------------------------------------------


//Member Variables--------------------------------------------------------------
LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display


//Setup-------------------------------------------------------------------------
void setup() {
  //feedback LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(9600);
  //Specify which program is currently running on Arduino
  Serial.println(F("START " __FILE__ " from " __DATE__ "\r\n"));

  //initialise LCD and turn on backlight
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

  Serial.println("Setup complete");
  Serial.println();
}



//Loop--------------------------------------------------------------------------
void loop() {
  //write to lcd from serial input testing:
  //re-create string each loop (better to make global and set to empty each time?)
  String inputString;

  if (Serial.available()) {
    Serial.println("debug: serial input");

    //delay to allow entire message to arrive
    delay(100);
    //remove all previous display
    lcd.clear();
    //redisplay instruction line and move cursor to second line for input display
    lcd.print("Serial input:");
    lcd.setCursor(0, 1);

    char inchar;
    while (Serial.available() > 0) {
      //input character from incoming serial
      inchar = Serial.read();
      //restrict input characters to text only (remove issues such as newline charater from attempting to print)
      if ((' ' <= inchar) && (inchar <= '~'))
        //build string to print
        inputString += inchar;
    }
    lcd.print(inputString);
  }

}


//Functions---------------------------------------------------------------------
