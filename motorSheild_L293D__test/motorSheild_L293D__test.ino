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
    v2.0:
      Mabuchi RF-300C-11440:
        + speed control test successful.
        + using new library.
        + testing with Mitsumi M35SP-7N.
        + limited difference seen at lower speeds (unloaded)
  TODO:
    + Test BRAKE capability under load/momentum
    + Add tests for stepper motors (assume replacement/additional libraries may be needed)
  v.20
    + Look into Adafruit accelStepper?
    + Find limits of library functions (ie. max speed parameters etc)
  ----------------------------------------------------------------------------*/

//Header files------------------------------------------------------------------
//lib for v1.0: possibly better than AFMotor for DC motors (need to test BRAKE function)
#include<MotorDriver.h>
//lib for v2.0 (stepper motor support)
#include<AFMotor.h>

//Member Variables--------------------------------------------------------------
//DC motor 1
AF_DCMotor dcMotor(1);
//spin up/down speed counter
uint8_t speedCounter;


//Setup-------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  Serial.println("Setup complete");

  //turn on motor (zero speed)
  dcMotor.setSpeed(200);
  dcMotor.run(RELEASE);
}


//Loop--------------------------------------------------------------------------
void loop() {
  //FORWARD TEST--------------------------
  dcMotor.run(FORWARD);

  //spin up motor incrementally
  Serial.println("motor 1: forward: spin UP");
  for (speedCounter = 0; speedCounter < 255; speedCounter++) {
    dcMotor.setSpeed(speedCounter);
    printSpeed();
    delay(50);
  }
  Serial.println("spin up complete.");
  delay(1000);

  //spin down motor decrementally
  Serial.println("motor 1: forward: spin DOWN");
  for (speedCounter = 255; speedCounter != 0; speedCounter--) {
    dcMotor.setSpeed(speedCounter);
    printSpeed();
    delay(50);
  }
  Serial.println("spin down complete.");
  delay(1000);


  //REVERSE TEST--------------------------
  dcMotor.run(BACKWARD);

  //spin up motor incrementally
  Serial.println("motor 1: backward: spin UP");
  for (speedCounter = 0; speedCounter < 255; speedCounter++) {
    dcMotor.setSpeed(speedCounter);
    printSpeed();
    delay(50);
  }
  Serial.println("spin up complete.");
  delay(1000);

  //spin down motor decrementally
  Serial.println("motor 1: backward: spin DOWN");
  for (speedCounter = 255; speedCounter != 0; speedCounter--) {
    dcMotor.setSpeed(speedCounter);
    printSpeed();
    delay(50);
  }
  Serial.println("spin down complete.");

  //STOP MOTOR----------------------------
  Serial.println("releasing motor 1");
  dcMotor.run(RELEASE);
  delay(3000);



  //
  //  Serial.println("motor 1: ON: 5 seconds");
  //  motorDriver.motor(1, FORWARD, 255);
  //  delay(5000);
  //
  //  Serial.println("motor 1: BRAKE: 5 seconds");
  //  motorDriver.motor(1, BRAKE, 255);
  //  delay(5000);
  //
  //  Serial.println("motor 1: BACKWARD: 5 seconds");
  //  motorDriver.motor(1, BACKWARD, 255);
  //  delay(5000);
  //
  //  Serial.println("motor 1: RELEASE: 5 seconds");
  //  motorDriver.motor(1, RELEASE, 255);
  //  delay(5000);
  //
  //  ceaseAll();
}



//Functions---------------------------------------------------------------------

// print motor speed (25%,50%,75%)
void printSpeed() {
  if (speedCounter == 63) {
    Serial.println("speed: 25%");
  } else if (speedCounter == 127) {
    Serial.println("speed: 50%");
  } else if (speedCounter == 191) {
    Serial.println("speed: 75%");
  }
}

//--------------------------------------
// release all motors before ceasing all activity indefinitely
//--------------------------------------
void ceaseAll() {
  //short brake all motors in case any are still moving)
  Serial.println("ceaseAll: brake all motors (1 second)");
  //  motorDriver.motor(1, BRAKE, 255);
  //  motorDriver.motor(2, BRAKE, 255);
  //  motorDriver.motor(3, BRAKE, 255);
  //  motorDriver.motor(4, BRAKE, 255);
  delay(1000);

  //release all motors
  Serial.println("ceaseAll: release all motors.");
  //  motorDriver.motor(1, RELEASE, 0);
  //  motorDriver.motor(2, RELEASE, 0);
  //  motorDriver.motor(3, RELEASE, 0);
  //  motorDriver.motor(4, RELEASE, 0);

  //persist in current state indefinitely
  Serial.println("ceaseAll: exit: Press reset button to repeat cycle.");
  //time to complete Serial output before exit
  delay(500);
  exit(0);
}
