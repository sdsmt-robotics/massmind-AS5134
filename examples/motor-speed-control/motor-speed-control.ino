/* 1/9/2020
 * Samuel Ryckman
 * 
 * Example of using the encoder to control a motor. This uses the 
 * Wingoneer BTS7960 motor controller, but very similar code to 
 * this will work for any pwm motor controller.
 */

#include <BTS7960.h>
#include "AS5134.h"

#define INIT_POWER 20
#define MAX_SPEED 30


//Create the motor
BTS7960 motor(40, 42, 44, true);

//Create the encoder object
AS5134 encoder(5, 6, 7);

//Define constants for the control
const double k = 0.3;
const int minPower = 30;
const int maxPower = 40;
const int proportionalThresh = 1080;
const int lowSpeedThresh = 360;
const int stopThresh = 25;

//Do some preliminary calculations for the proportional zone
const float proportionalMult = float(maxPower - minPower) / (proportionalThresh - lowSpeedThresh);

//Where we want to be
const int initialTarget = 0;

int getSpeed() {
  static unsigned long curTime, lastTime = micros();
  static int curPos, lastPos = encoder.readMultiTurnAngle();
  static int curSpeed;

  //Calculate the speed based on the current position
  curPos = encoder.readMultiTurnAngle();
  curTime = micros();
  curSpeed = long(curPos - lastPos) * 166667 / (curTime - lastTime); //rpm
  //curSpeed = (curPos - lastPos); //rpm
  
  encoder.resetCounter();
  lastPos = encoder.readMultiTurnAngle();
  lastTime = micros();
  
  return curSpeed;
}

int filterSpeed(int newVal) {
  static int pastVals[] = {0, 0, 0, 0, 0};
  const int numVals = 5;
  static int average = 0;
  static int curIndex = 0;
  long total = 0;

  //add the new value
  pastVals[curIndex] = newVal;

  //update position in the rolling array
  curIndex++;
  if (curIndex >= numVals) {
    curIndex = 0;
  }

  //get the total
  for (int i = 0; i < numVals; i++) {
    total += pastVals[i];
  }

  //return the average
  return total / numVals;
}

int computeOutput(int curVal) {
  //Define some constants
  const int maxOutput = 255;    //max output value
  const int minOutput = -255;   //min output value
  const int maxOutCap = 40;     //max output we will send
  const int minOutCap = -40;    //min output we will send
  const int maxInput  = 18000;  //max possible input value
  const int minInput  = -18000; //min possible input value
  static long lastTime = 0;
  static int lastError = 0;
  long curTime = micros();

  //Define PID constants
  const float kp = (float)abs(maxOutput - minOutput) / abs(maxInput - minInput);
  const float ki = 1;
  const float kd = 0.004;
  
  const int target = 1000;

  static float integral = 0;
  float derivative = 0;
  
  int output = 0;
  int error = 0;
  float elapsedTime;

  
  // Skip if first loop so we can get initial values
  if (lastTime == 0) {
    lastTime = micros();
    lastError = target - curVal;
    return (maxOutput + minOutput) / 2;
  }

  //update the time
  elapsedTime = (curTime - lastTime) / 1000000.0;
  lastTime = curTime;

  //get the error
  error = target - curVal;

  //Get the integral of the error
  integral += (float)error * elapsedTime;

  //Get the derivative of the error
  derivative = (float)(error - lastError) / elapsedTime;

  //calculate the output
  output = kp * error + ki * integral + kd * derivative;

  //cap the output and return
  return constrain(output, minOutCap, maxOutCap);
}


//=====setup==============================
void setup() {
  //Start serial
  Serial.begin(38400);
  
  //Initialize the motor
  motor.init();
  motor.stop();
  motor.run(INIT_POWER);
  
  //Initialize the encoder
  encoder.init();


}

//=====loop==============================
void loop() {
  static bool motorRunning = true;
  static int motorPower = INIT_POWER;
  static int motorSpeed;
  static int motorTarget = 2000;
  static int lastPrint = millis();
  
  //Read input if it is available
  if (Serial.available() > 0) {
    char readVal = Serial.read();
    while(Serial.available()){Serial.read();}
  
    switch (readVal) {
      case ' ':
        //toggle the motor on or off
        if (motorRunning) {
          motor.stop();
          motorRunning = false;
        } else {
          motor.run(motorPower);
          motorRunning = true;
        }
        break;
      case '+':
      case '=':
        motorPower += 5;
        motorRunning = true;
        motor.run(motorPower);
        break;
      case '-':
      case '_':
        motorPower -= 5;
        motorRunning = true;
        motor.run(motorPower);
        break;
      default:
        break;
    }
  }

  //read speed and adjust power accordingly
  motorSpeed = getSpeed();
  motorSpeed = filterSpeed(motorSpeed);
  motorPower = computeOutput(motorSpeed);
  
  motor.run(motorPower);
  
  //Output current speed
  if (millis() - lastPrint > 20) {
    Serial.print("Power: ");
    Serial.print(motorPower);
    Serial.print(", Speed: ");
    Serial.println(motorSpeed);
    lastPrint = millis();
  }
}
