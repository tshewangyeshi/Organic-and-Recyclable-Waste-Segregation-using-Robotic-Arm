#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Create an instance of the Adafruit PWM Servo Driver
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define NUM_SERVOS 5
#define SERVO_MIN 0   // Minimum pulse length
#define SERVO_MAX 600 // Maximum pulse length
#define SERVO_DELAY 100 // Delay between movements

// Define servo pins
int servoPins[NUM_SERVOS] = {0, 1, 2, 3, 5}; // Excluding pin 4 as it's spoiled

void setup() {
  Serial.begin(9600);
  Serial.println("Servo control ready");

  pwm.begin();
  pwm.setPWMFreq(50);  // Set PWM frequency to 50 Hz

  // Initial positions for servos
  pwm.setPWM(0, 0, 380); // Claw
  // pwm.setPWM(1, 0, 590); // End effector holder
  // pwm.setPWM(2, 0, 590); // Holder for servo 1
  pwm.setPWM(3, 0, 590); // Frame holder
  pwm.setPWM(5, 0, 380); // Base
}

void moveClawOpen() {
  for (int pos = 250; pos <= 380; pos++) {
    pwm.setPWM(0, 0, pos); // Claw open
    delay(15);
  }
}

void moveClawClose() {
  for (int pos = 380; pos >= 250; pos--) {
    pwm.setPWM(0, 0, pos); // Claw close
    delay(8);
  }
}

// void moveEndEffectorLeft() {
//   for (int pos = 590; pos >= 300; pos--) {
//     pwm.setPWM(1, 0, pos); // End effector holder move left
//     delay(8);
//   }
// }

//  void moveEndEffectorRight() {
//    for (int pos = 300; pos <= 590; pos++) {
//      pwm.setPWM(1, 0, pos); // End effector holder move right
//      delay(8);
//    }
//  }

//  void twistHolderLeft() {
//   for (int pos = 590; pos >= 300; pos--) {
//      pwm.setPWM(2, 0, pos); // Holder twist left
//      delay(8);
//    }
//  }

//  void twistHolderRight() {
//   for (int pos = 300; pos <= 590; pos++) {
//      pwm.setPWM(2, 0, pos); // Holder twist right
//      delay(8);
//    }
//  }

void moveArmUp() {
  for (int pos = 350; pos <= 590; pos++) {
    pwm.setPWM(3, 0, pos); // Arm down
    delay(8);
  }
}

void moveArmDown() {
  for (int pos = 590; pos >= 350; pos--) {
    pwm.setPWM(3, 0, pos); // Arm up
    delay(8);
  }
}

void moveBaseRight() {
  for (int pos = 380; pos >= 120; pos--) {
    pwm.setPWM(5, 0, pos); // Base turn left
    delay(8);
  }
}
void moveBaseLeftFromRight(){
  for (int pos = 120; pos <= 380; pos++) {
    pwm.setPWM(5, 0, pos); // Base turn right
    delay(8);
  }
}
void moveBaseLeft() {
  for (int pos = 380; pos <= 600; pos++) {
    pwm.setPWM(5, 0, pos); // Base turn right
    delay(8);
  }
}
void moveBaseRightFromLeft(){
  for (int pos = 600; pos >= 380; pos--) {
    pwm.setPWM(5, 0, pos); // Base turn left
    delay(8);
  }
}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    
    if (command == 'L') {
      Serial.println("Executing left (organic) sequence");
      
      // Sequence for moving to the left (organic)
      moveArmDown();
      moveClawClose();
      moveArmUp();
      moveBaseLeft();
      moveArmDown();
      moveClawOpen();
      moveArmUp();
      moveBaseRightFromLeft();
    }
    else if (command == 'R') {
      Serial.println("Executing right (recycle) sequence");
      
      // Sequence for moving to the right (recycle)
      moveArmDown();
      moveClawClose();
      moveArmUp();
      moveBaseRight();
      moveArmDown();
      moveClawOpen();
      moveArmUp();
      moveBaseLeftFromRight();
    }
    else {
      Serial.println("Unknown command received");
    }
  }
}
