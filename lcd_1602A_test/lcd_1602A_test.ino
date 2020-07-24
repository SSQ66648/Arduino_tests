/*------------------------------------------------------------------------------
  PROJECT:      Module tests
  FILE:         lcd_1602A_test.ino
  AUTHOR:       SSQ66648
  VERSION:      v1.0
  DESCRIPTION:  Testing and experimentation of Arduino LCD module: 1602A
  ------------------------------------------------------------------------------
  NOTES:
    + LCD unit used includes integrated I2C conversion board.
    + Did not include potentiometer for testing.
  TESTING RESULTS:
    v1.0:
      adjustment of contrast screw must be first troubleshooting step on first connection.
      rightToLeft() produces mirror-writing.
      setCursor() controls direction of auto-scrolling (ie 0,0 right-to-left and anything else(?) left-to-right).
        -above is observed in exeriments with own newText() method. NOT during scrolling-string-test-text (right-to-left as normal).
      scrolling text is capable of exacly 40 characters per line.
      scrolling strings longer than 40char will position on next line exactly (ie char 40 directly below char 0)
      minimum loop delay for text rendering is found to be ~500ms. any lower and text becomes blurry.
      scrolling strings >80 characters will display overwriting prvious text (ie test string I-9+ replaces A-9+)
        -Original text (A-9) is not displayed correctly before being replaced by scrolling characters 80+ (does not "loop", only overwrites).
      scrolling requires active printing ("set characters" comment) otherwise no scolling occurrs.
      scrolling test string is missing character (overwritten by cursor position printing empty character to drive loop)
        -however this is NOT the same behaviour as scrolling strings: as cursor character is written each loop, original character is NOT displayed before overwrite.
        -cursor position  behaviour does not conform to expected pattern when pushed beyond 40 character limit of lines.
          expected behaviour found:   character 40 line 0 (final character of line should be 39) will push to character 0 line 1.
          unexpected behaviour found: character 80 line 0 should loop back to character 0 line 0 (if 40,0 is equal to 0,1)
            BUT actually overwrites character 56 line 0? (ie F-6).
            (see file cursorLogicTest.txt for full testing results)
  TODO:
    + Serial input to LCD output
    + add potentiometer to dim backlight
    + add to RFID sketch to display chip UUID
  (lesser) TODO:
    +
  ----------------------------------------------------------------------------*/

//Header files------------------------------------------------------------------
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
  //show visible cursor on screen
  lcd.cursor();

  lcd.begin(16, 1);
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0, 0);
  //test string to find limits of scrolling behaviour
  lcd.print("A123456789B123456789C123456789D123456789E123456789F123456789G123456789H123456789I123456789");
  lcd.autoscroll();    // Set diplay to scroll automatically


  Serial.println("Setup complete");
  Serial.println();

  //delay to see first character before scrolling
  delay(2000);
}


String message;


//Loop--------------------------------------------------------------------------
void loop() {
  //test of passing string to display
  // message = "test";
  //  newText(message);
  //  newText("this");

  //  newText("sixteencharacter");
  // newText("over sixteen characters");

  //  newText("still over sixteen characters");
  //  delay(2000);


  lcd.setCursor(70, 0);
  lcd.print(" ");      // set characters
  delay(500);

}





//Functions---------------------------------------------------------------------
void newText(String message) {
  //clear and set cursor to start (remove need to repeat this each time)
  lcd.clear();
  lcd.print(message);
  delay(2000);
}
