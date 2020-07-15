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
    v3.0:
      Mitsumi M35SP-7N:
        + unsure of exact voltage rating (6 or 24v)
        + test successful from L293D supply (5v, max0.6A)
        + VERY slightly under 360 rotation at 48steps, noticably over at 49
        + trial and error to find effective RPM. max values found to complete full rotation:
            single coil   <= 400
            double coil   <= 400
            interleave    <= 500
            microstep     <= 750
  TODO:
    + Test BRAKE capability under load/momentum
    + Add tests for stepper motors (assume replacement/additional libraries may be needed)
  v2.0
    + Look into Adafruit accelStepper?
    + Find limits of library functions (ie. max speed parameters etc)
  v2.1
    + Rewrite step tests to own methods (type passed as argument)
    + Add servo tests when purchased
  ----------------------------------------------------------------------------*/

//Header files------------------------------------------------------------------
//lib for v1.0: possibly better than AFMotor for DC motors? (need to test BRAKE function)
#include<MotorDriver.h>
//lib for v2.0+ (stepper motor support)
#include<AFMotor.h>

//Member Variables--------------------------------------------------------------
//DC motor 1
AF_DCMotor dcMotor(1);
//stepper motor (number of steps in 360 rotation (aka 7.5 degree steps), sheild port no (M3&M4))
AF_Stepper stepMotor(48, 2);

//spin up/down speed counter
uint8_t speedCounter;


//Setup-------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  Serial.println("Setup complete");

  //turn on dc motor (zero speed)
  dcMotor.setSpeed(100);
  dcMotor.run(RELEASE);

  //set RPM speed of stepper motor
  stepMotor.setSpeed(750);
}


//Loop--------------------------------------------------------------------------
void loop() {
  //DC FORWARD TEST-----------------------
  Serial.println("DC motor test: forward");
  dcMotor.run(FORWARD);
  dcSpinupTest();

  //DC BACKWARD TEST----------------------
  Serial.println("DC motor test: backward");
  dcMotor.run(BACKWARD);
  dcSpinupTest();

  //STOP DC MOTOR-------------------------
  Serial.println("releasing motor 1");
  dcMotor.run(RELEASE);
  delay(1000);

  //STEP *1 COIL--------------------------
  stepMotor.setSpeed(400);
  Serial.println("Stepper motor: single coil test: forward");
  stepMotor.step(48, FORWARD, SINGLE);
  delay(1000);
  Serial.println("Stepper motor: single coil test: backward");
  stepMotor.step(48, BACKWARD, SINGLE);
  delay(2000);

  //STEP *2 COIL--------------------------
  stepMotor.setSpeed(400);
  Serial.println("Stepper motor: double coil test: forward");
  stepMotor.step(48, FORWARD, DOUBLE);
  delay(1000);
  Serial.println("Stepper motor: double coil test: backward");
  stepMotor.step(48, BACKWARD, DOUBLE);
  delay(2000);

  //STEP INTERLEAVE-----------------------
  stepMotor.setSpeed(500);
  //greater accuracy (ie 96 steps ber turn)
  Serial.println("Stepper motor: interleaved coil test: forward");
  stepMotor.step(96, FORWARD, INTERLEAVE);
  delay(1000);
  Serial.println("Stepper motor: interleaved coil test: backward");
  stepMotor.step(96, BACKWARD, INTERLEAVE);
  delay(2000);

  //STEP MICROSTEP------------------------
  stepMotor.setSpeed(750);
  Serial.println("Stepper motor: microstep test: forward");
  stepMotor.step(48, FORWARD, MICROSTEP);
  delay(1000);
  Serial.println("Stepper motor: microstep test: backward");
  stepMotor.step(48, BACKWARD, MICROSTEP);
  delay(2000);

  Serial.println("releasing step motor");
  stepMotor.release();

  //STOP PROGRAM--------------------------
  Serial.println("end loop: press reset button to repeat cycle.");
  delay(100);
  exit(0);

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

void dcSpinupTest() {
  //spin up motor incrementally
  Serial.println("motor 1: spin UP");
  for (speedCounter = 0; speedCounter < 255; speedCounter++) {
    dcMotor.setSpeed(speedCounter);
    printSpeed();
    delay(50);
  }
  Serial.println("spin up complete.");
  delay(1000);

  //spin down motor decrementally
  Serial.println("motor 1: spin DOWN");
  for (speedCounter = 255; speedCounter != 0; speedCounter--) {
    dcMotor.setSpeed(speedCounter);
    printSpeed();
    delay(50);
  }
  Serial.println("spin down complete.");
  delay(1000);
}

//obsolete (todo: rewrite for current library)
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
