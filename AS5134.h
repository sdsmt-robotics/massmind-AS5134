/* 1/10/2020
 * Samuel Ryckman
 * 
 * Header for the Massmind AS5134 class.
 */

#ifndef AS5134_H
#define AS5134_H

#include "Arduino.h"

class AS5134
{
public:
    AS5134(int dioPin, int csPin, int clkPin);
  
    void init();
    int readCounter();
    void resetCounter();
    int readAngle();
    long readMultiTurnAngle();
  
private:
    int transmit(int command);
    void transmit(int command, int data);
    int transmit(int command, bool sendMode, int data = 0);
    
    int dioPin, csPin, clkPin;
};

#endif