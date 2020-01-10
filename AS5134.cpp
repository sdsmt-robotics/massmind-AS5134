#include "AS5134.h"

//Define some commands
#define RD_ANGLE 0
#define RD_COUNTER 4
#define SET_COUNTER 20
#define WRITE_CONFIG 23

/** Constructor for the class.
*/
AS5134::AS5134(int dioPin, int csPin, int clkPin)
    : dioPin(dioPin),
      csPin(csPin),
      clkPin(clkPin)
{ }


/**
* Initialize the pins.
*/
void AS5134::init() {
    pinMode(csPin,  OUTPUT);
    pinMode(clkPin, OUTPUT);
    pinMode(dioPin, INPUT);
    
    digitalWrite(csPin, LOW);
    digitalWrite(clkPin, LOW);
}


/**
* Read the multi-turn counter.
* 
* @return the number of complete rotations.
*/
int AS5134::readCounter() {
    return transmit(RD_COUNTER) >> 7;
}


/**
* Reset the multi-turn counter to 0.
*/
void AS5134::resetCounter() {
    transmit(SET_COUNTER, 0);
}


/**
* Read the absolute angle value.
* 
* @return the absolute angle value.
*/
int AS5134::readAngle() {
    const int ANGLE_MASK = 0b111111111;
    
    return transmit(RD_ANGLE) & ANGLE_MASK;
}


/**
* Read the total angle turned from original zero position.
* 
* @return the total angle value.
*/
long AS5134::readMultiTurnAngle() {
    int turns = readCounter();
    int angle = readAngle();
    
    return (long)turns * 360 + angle;
}


/**
* Data transmission with the encoder for reading data
* 
* @param command - Data transmission command.
* @return the data received.
*/
int AS5134::transmit(int command) {
    return transmit(command, false);
}

/**
* Data transmission with the encoder for sending data
* 
* @param command - Data transmission command.
* @param data - Data to send to the encoder.
*/
void AS5134::transmit(int command, int data) {
    transmit(command, true, data);
}

/**
* Data transmission with the encoder for sending or reading data
* 
* @param command - Data transmission command.
* @param sendMode - Set whether we are sending or reading data.
* @param data - Data to send to the encoder.
* @return the data received if in read mode.
*/
int AS5134::transmit(int command, bool sendMode, int data = 0) {
    
    // Start the transmission
    digitalWrite(csPin,LOW);
    digitalWrite(clkPin,LOW);
    digitalWrite(csPin,HIGH);
    delayMicroseconds(1);

    //send the command
    pinMode(dioPin, OUTPUT);
    for(int bitNum = 4; bitNum >= 0; bitNum--) {
        //tick clock low
        digitalWrite(clkPin,LOW);
        delayMicroseconds(1);

        //send the bit
        digitalWrite(dioPin, ((command >> bitNum) & 1));

        //tick clock high
        digitalWrite(clkPin,HIGH);
        delayMicroseconds(1);
    }

    //read/write the data
    if (!sendMode) {
        pinMode(dioPin, INPUT);
    }
    for(int bitNum=15; bitNum >= 0; bitNum--) {
        //tick the clock LOW
        digitalWrite(clkPin,LOW);
        delayMicroseconds(1);
        
        //send the bit if in send mode
        if (sendMode) {
            digitalWrite(dioPin, ((command >> bitNum) & 1));
        }
        
        //tick the clock HIGH
        digitalWrite(clkPin,HIGH);
        delayMicroseconds(1);

        //Add the bit to the response if in read mode
        if (!sendMode) {
            if(digitalRead(dioPin)) {
              data += (1 << (bitNum));
            }
        }
    }

    //End the transmission
    digitalWrite(clkPin, LOW);
    digitalWrite(csPin, LOW);

    return data;
}