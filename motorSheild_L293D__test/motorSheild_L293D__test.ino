/*------------------------------------------------------------------------------
  PROJECT:      Module tests
  FILE:         motorSheild_L293D_test.ino
  AUTHOR:       SSQ66648
  VERSION:      v1.0
  DESCRIPTION:  Testing and experimentation of Arduino motor control sheild: L293D
  ------------------------------------------------------------------------------
  NOTES:
    + Serial output has been included throughout for debugging/testing
    + Function ceaseAll() has been added to prevent endless on-off cycle during testing.
  TESTING RESULTS:
    v1.0:
      Mabuchi RF-300C-11440 D/V5.9:
        + tests successful.
        + powered by Arduino Uno clone via inplace 5v board jumper (datasheet 1.5-6v, 3v nominal).
        + BRAKE did not offer any noticable resistance to external manipulation.
  TODO:
    + Test BRAKE capability under load/momentum
    + Add tests for stepper motors (assume replacement/additional libraries may be needed)
  ----------------------------------------------------------------------------*/

//Header files------------------------------------------------------------------
#include<MotorDriver.h>


//Member Variables--------------------------------------------------------------
MotorDriver motorDriver;


//Setup-------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  Serial.println("Setup complete");
}


//Loop--------------------------------------------------------------------------
void loop() {
  Serial.println("motor 1: ON: 5 seconds");
  motorDriver.motor(1, FORWARD, 255);
  delay(5000);

  Serial.println("motor 1: BRAKE: 5 seconds");
  motorDriver.motor(1, BRAKE, 255);
  delay(5000);

  Serial.println("motor 1: BACKWARD: 5 seconds");
  motorDriver.motor(1, BACKWARD, 255);
  delay(5000);

  Serial.println("motor 1: RELEASE: 5 seconds");
  motorDriver.motor(1, RELEASE, 255);
  delay(5000);

  ceaseAll();
}



//Functions---------------------------------------------------------------------

//--------------------------------------
// release all motors before ceasing all activity indefinitely
//--------------------------------------
void ceaseAll() {
  //short brake all motors in case any are still moving)
  Serial.println("ceaseAll: brake all motors (1 second)");
  motorDriver.motor(1, BRAKE, 255);
  motorDriver.motor(2, BRAKE, 255);
  motorDriver.motor(3, BRAKE, 255);
  motorDriver.motor(4, BRAKE, 255);
  delay(1000);

  //release all motors
  Serial.println("ceaseAll: release all motors.");
  motorDriver.motor(1, RELEASE, 0);
  motorDriver.motor(2, RELEASE, 0);
  motorDriver.motor(3, RELEASE, 0);
  motorDriver.motor(4, RELEASE, 0);

  //persist in current state indefinitely
  Serial.println("ceaseAll: exit: Press reset button to repeat cycle.");
  //time to complete Serial output before exit
  delay(500);
  exit(0);
}
