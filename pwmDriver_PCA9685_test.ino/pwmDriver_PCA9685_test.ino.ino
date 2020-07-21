/*------------------------------------------------------------------------------
  PROJECT:      Module tests
  FILE:         pwmDriver_PCA9685_test.ino
  AUTHOR:       SSQ66648
  VERSION:      v1.0
  DESCRIPTION:  Testing and experimentation of Arduino pulse-width modulation board: PCA9685
  ------------------------------------------------------------------------------
  NOTES:
    + Serial output has been included throughout for debugging/testing
    + Board tested with TowerPro Micro 9g servo SG90 (*1 then *10)
    + test of using on-board power supply only has been dropped due to potential of brown-outs or overheating issue
    + serial input of (1) to proceed is a plcaceholder and will be replaced with a physical button to proceed.
  TESTING RESULTS:
    v1.0:
      scanning from 0-180 or back incrementally has resulted in non-full rotation in two tested servos.
  TODO:
    + add button press between actions
    + test single servo
    + add use of PWM board with one servo
    + test all 10 servos simultaniously
    + obtain and test with LEDs
    + recreate test program in python and port to Raspberry Pi
    + return to ServoEasing.h library (or similar) to fine-tune movement
    + chain drivers to support multiple boards when obtained
  (lesser) TODO:
    + remove duplicate '.ino' from filename of setup serial output
    + reduce full path to just filename in setup identification
  ----------------------------------------------------------------------------*/

//Header files------------------------------------------------------------------
//#include "ServoEasing.h"    //consider changing to this library
#include <Servo.h>


//Definitions-------------------------------------------------------------------


//Member Variables--------------------------------------------------------------
Servo servo0;
//starting angle
int angle = 0;
//counter to iterate through available servos (unused)
uint8_t servonum = 0;
//(poor) user-control of program progression via serial input (to be replaced with physical button asap)
int input = 0;


//Setup-------------------------------------------------------------------------
void setup() {
  //feedback LED
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
  //Specify which program is currently running on Arduino
  Serial.println(F("START " __FILE__ " from " __DATE__ "\r\n"));

  //assign servo
  servo0.attach(8);
  Serial.println ("Servos set to zero");
  servo0.write(angle);

  Serial.println("Setup complete");
}


//Loop--------------------------------------------------------------------------
void loop() {
  //scan input control-----------
  //scanning servos: test actually turns full 180 (<180 found every time)
  Serial.println ("enter 1 to set servos to 180");
  while (input != 1) {
    input = Serial.parseInt();
  }
  input = 0;
  // scan from 0 to 180 degrees
  for (angle = 0; angle <= 190; angle++)
  {
    Serial.println(angle);
    servo0.write(angle);
    delay(50);
  }

  //test angle actually returns to zero from full 180
  Serial.println ("enter 1 to set servo to 0");
  while (input != 1) {
    input = Serial.parseInt();
  }
  input = 0;
  for (angle = 180; angle > 0; angle--)
  {
    Serial.println(angle);
    servo0.write(angle);
    delay(5);
  }

  //manual input control----------
  Serial.println ("enter 1 to set servo to manual 180");
  while (input != 1) {
    input = Serial.parseInt();
  }
  input = 0;
  servo0.write(180);

  Serial.println ("enter 1 to set servo to manual 90");
  while (input != 1) {
    input = Serial.parseInt();
  }
  input = 0;
  servo0.write(90);

  Serial.println ("enter 1 to set servo to manual 0");
  while (input != 1) {
    input = Serial.parseInt();
  }
  input = 0;
  servo0.write(0);


}



//Functions---------------------------------------------------------------------
