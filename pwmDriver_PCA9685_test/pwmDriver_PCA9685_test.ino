/*------------------------------------------------------------------------------
  PROJECT:      Module tests
  FILE:         pwmDriver_PCA9685_test.ino
  AUTHOR:       SSQ66648
  VERSION:      v3.0
  DESCRIPTION:  Testing and experimentation of Arduino pulse-width modulation board: PCA9685
  ------------------------------------------------------------------------------
  NOTES:
    + Serial output has been included throughout for debugging/testing
    + Board tested with TowerPro Micro 9g servo SG90 (*1 then *10)
    + test of using on-board power supply only has been dropped due to potential of brown-outs or overheating issue
    + serial input of (1) to proceed is a plcaceholder and will be replaced with a physical button to proceed.
  TESTING RESULTS:
    v1.0:
      Scanning from 0-180 or back incrementally has resulted in non-full rotation in two tested servos.
      Setting to angle value results in snapping to correct angle despuite scanning issues.
    v2.0:
      Written off due to multiple issues presumed to be caused by library incompatibilities. (multiple implementations attempted with different libraries)
    v3.0:
      Currently cannot quickly "snap-to-angle" as could previously when controlled directly from Arduino.
        - no librbary method allows for this behaviour when using the PWM board.
        - requires "scanning" movement for-loop to work
        - attempts at removing the loop and recreating the "snap-to-start-angle" behaviour seen at the start of the loop have been unsucessful.
      SERVOMIN and SERVOMAX values obtained for SG90 servos through trial and error.
        - no difference between 105 and 100 for minimum angle, or 490 and 500 for max angle
        - decided against rounding the numbers as this may introduce uneeded stress on the (nylon) gearing
      Based off same example code as one of the (completely unresponsive) 2.0 iterations, but works fine.
      Has been implemented on the Nano rather than the Uno (board choice possibly related to 2.0 issues?)
      All ten servos are responive to test code.
      Servo #4 is noticeably slower than others (others slightly slower) and not all complete a full 180 (appx ~170)
        - assumed to be related to low quality of hardware construction rather than code issues.
      Development of current version has been noted to occasionally require power reset before physical movement begins
        - not asssumed to be physical jamming as this would not prevent the next servo activating.
  TODO:
    + add button press between actions
    + write methods / define fixed angles to rotate to
    + obtain and test with LEDs
    + recreate test program in python and port to Raspberry Pi
    + return to ServoEasing.h library (or similar) for fine-tuned movements
    + chain drivers to support multiple boards when obtained
  (lesser) TODO:
    + remove duplicate '.ino' from filename of setup serial output
    + reduce full path to just filename in setup identification
    + add debugging enabled/disabled outputs (ie using <0 in for-loop)
  ----------------------------------------------------------------------------*/

//Header files------------------------------------------------------------------
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
//consider changing to this library:
//#include "ServoEasing.h"


//Definitions-------------------------------------------------------------------
//trial-and-error found values found for specific servos used (before hitting hard stop limits)
#define SERVOMIN  105 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  490 // This is the 'maximum' pulse length count (out of 4096)
//unused demo values copied here for future reference
#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
//analog servo frequency (attempts at adjusting this have resulted in unresponsive servos, even after correcting back to 50)
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

//"shortcut" angle values (in degrees 0-180)
#define degree0 105
#define degree90 297  //rounded value (half-way between min & max  values)
#define degree180 490


//Member Variables--------------------------------------------------------------
//uses default address (0x40) - other options available -see lib docs
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();


//Setup-------------------------------------------------------------------------
void setup() {
  //feedback LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(9600);
  //Specify which program is currently running on Arduino
  Serial.println(F("START " __FILE__ " from " __DATE__ "\r\n"));

  //values taken from lib example and seem to work (requires oscilloscope to find exact values for specific chip used)
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);
  delay(10);

  Serial.println("Setup complete");
}


//Loop--------------------------------------------------------------------------
void loop() {
  //set servos #0-9 to MAX position
  for (uint16_t servonum = 0; servonum < 10; servonum++) {
    Serial.print("MAX position: Servo ");
    Serial.println(servonum);
    for (uint16_t pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen++) {
      pwm.setPWM(servonum, 0, pulselen);
    }
    delay(500);
  }
  delay(2000);

  //set servos #0-9 to MIN position
  for (uint16_t servonum = 0; servonum < 10; servonum++) {
    Serial.print("MIN position: Servo ");
    Serial.println(servonum);
    for (uint16_t pulselen = SERVOMAX; pulselen > SERVOMIN; pulselen--) {
      pwm.setPWM(servonum, 0, pulselen);
    }
    delay(500);
  }
  delay(2000);



}



//Functions---------------------------------------------------------------------
