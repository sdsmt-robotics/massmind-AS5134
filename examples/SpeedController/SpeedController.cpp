/* 1/18/2020
 * Christian Weaver & Samuel Ryckman
 *
 * SpeedController class.
 * This file provides function definitions to regulate the power outputted to a motor to maintain a given speed.
 */

#include "SpeedController.h"


//forces the output to be between minOutCap and maxOutCap
int constrain(int input, int minOutCap, int maxOutCap)
{
    if (input < minOutCap)
    {
        return minOutCap;
    }
    else if (input > maxOutCap)
    {
        return maxOutCap;
    }
    return input;
}


SpeedController::SpeedController(InitSpeedControllerValues initVals)
{
    int maxOutput = initVals.maxOutput;
    int minOutput = initVals.minOutput;
    int maxOutCap = initVals.maxOutCap;
    int minOutCap = initVals.minOutCap;
    int maxInput = initVals.maxInput;
    int minInput = initVals.minInput;
}


int SpeedController::motorSpeedToPower(int desiredMotorSpeed)
{
    int motorSpeed = filterMotorSpeed(curMotorSpeed);
    int motorPower = computeMotorPower(motorSpeed);

    return motorPower;
}


void SpeedController::updateCurState(int curPosition, int curMiliTime)
{
    this->curMiliTime = curMiliTime;

    this->curMotorPosition = curPosition;

    this->curMotorPosition = getSpeed();

    //shift the array of past motor speeds left one
    for (int i = 0; i < numSampledSpeeds - 1; i++)
    {
        sampledSpeeds[i] = sampledSpeeds[i + 1];
    }

    //put curSpeed in the rightmost spot in the array
    sampledSpeeds[numSampledSpeeds - 1] = curMotorSpeed;
}


int SpeedController::computeMotorPower(int motorSpeed)
{
    static long lastTime = 0;
    static int lastError = 0;
    long curTime = curMiliTime;

    //Define PID constants
    const float kp = (float)abs(maxOutput - minOutput) / abs(maxInput - minInput);
    const float ki = 1;
    const float kd = 0.004f;

    const int target = 1000;

    static float integral = 0;
    float derivative = 0;

    int output = 0;
    int error = 0;
    float elapsedTime;


    // Skip if first loop so we can get initial values
    if (lastTime == 0) {
        lastTime = curMiliTime;
        lastError = target - motorSpeed;
        return (maxOutput + minOutput) / 2;
    }

    //update the time
    elapsedTime = float((curTime - lastTime) / 1000000.0);
    lastTime = curTime;

    //get the error
    error = target - motorSpeed;

    //Get the integral of the error
    integral += (float)error * elapsedTime;

    //Get the derivative of the error
    derivative = (float)(error - lastError) / elapsedTime;

    //calculate the output
    output = int(kp * error + ki * integral + kd * derivative);

    //cap the output and return
    return constrain(output, minOutCap, maxOutCap);
}


int SpeedController::filterMotorSpeed(int curMotorSpeed)
{
    long total = 0;

    //get the total
    for (int i = 0; i < numSampledSpeeds; i++) {
        total += sampledSpeeds[i];
    }

    //return the average
    return total / numSampledSpeeds;
}


int SpeedController::getSpeed() {
    static unsigned long curTime, lastTime = curMiliTime;
    static int curPos, lastPos = curMotorPosition;
    static int curSpeed;

    //Calculate the speed based on the current position
    curPos = curMotorPosition;
    curTime = curMiliTime;
    curSpeed = long(curPos - lastPos) * 166667 / (curTime - lastTime); //rpm

    lastPos = curMotorPosition;
    lastTime = curMiliTime;

    return curSpeed;
}