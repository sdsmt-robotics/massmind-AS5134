/* 1/18/2020
 * Christian Weaver & Samuel Ryckman
 *
 * Header for the SpeedController class
 */

#ifndef SPEEDCONTROLLER
#define SPEEDCONTROLLER

#include <cmath>

using namespace std;

const int numSampledSpeeds = 5;

//This concisely stores the initialization values for the SpeedController constructor
struct InitSpeedControllerValues
{
    int maxOutput = 255;    //max output value
    int minOutput = -255;   //min output value
    int maxOutCap = 40;     //max output we will send
    int minOutCap = -40;    //min output we will send
    int maxInput = 18000;  //max possible input value
    int minInput = -18000; //min possible input value
};


class SpeedController
{
public:
    SpeedController(InitSpeedControllerValues initVals);
    
    int motorSpeedToPower(int desiredMotorSpeed);
    void updateCurState(int curPosition, int curMiliTime);


private:
    int maxOutput;  //max output value
    int minOutput;  //min output value
    int maxOutCap;  //max output we will send
    int minOutCap;  //min output we will send
    int maxInput;   //max possible input value
    int minInput;   //min possible input value
    int sampledSpeeds[numSampledSpeeds] = {0};
    int curMotorPosition = 0;
    int curMotorSpeed = 0;
    int curMiliTime = 0;

    int computeMotorPower(int motorSpeed);
    int filterMotorSpeed(int curMotorSpeed);
    int getSpeed();
};




#endif