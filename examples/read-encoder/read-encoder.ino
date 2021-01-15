/* 1/9/2020
 * Samuel Ryckman
 * 
 * Example of how to use the library.
 * Displays the turn count and current angle.
 *  - Spacebar: resets the turn count.
 *  - 's': toggles sleep mode.
 */

#include "AS5134.h"

//Create the encoder object
AS5134 encoder(5, 6, 7);


//=====setup==============================
void setup() {
  Serial.begin(38400);

  //Initialize the encoder
  encoder.init();
}

//=====loop==============================
void loop() {
  char readVal;
  static bool sleeping = false;

  //Output current state
  Serial.print("Turns: ");
  Serial.print(encoder.readCounter());
  Serial.print(", Angle: ");
  Serial.print(encoder.readAngle());
  Serial.print(", Sleeping: ");
  Serial.print(sleeping);
  Serial.print(", ADC_LOCK: ");
  Serial.println(encoder.getLockAdc());

  
  //Read value from console if we have one
  if (Serial.available() > 0) {
    readVal = Serial.read();

    //clear the rest
    while(Serial.available()){Serial.read();}

    if (readVal == ' ') {
      //reset the turn count
      encoder.resetCounter();
    } else if (readVal == 's') {
      sleeping = !sleeping;
      encoder.setLowPowerMode(sleeping);
    }
  }
  
  delay(10);
}
