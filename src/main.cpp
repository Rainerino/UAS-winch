//
// Created by yiyi on 17/03/19.
//

/*
 * Driver module
 */
#include "main.h"
#include "Arduino.h"
#include <Servo.h>



void setup() {

}

void loop() {

}

//
//
////
//// Created by yiyi on 17/03/19.
////
//
//#include "temp.h"
///* Encoder Library - Basic Example
// * http://www.pjrc.com/teensy/td_libs_Encoder.html
// *
// * This example code is in the public domain.
// */
//
//#include <Encoder.h>
//
//// Change these two numbers to the pins connected to your encoder.
////   Best Performance: both pins have interrupt capability
////   Good Performance: only the first pin has interrupt capability
////   Low Performance:  neither pin has interrupt capability
//Encoder myEnc(5, 6);
////   avoid using pins with LEDs attached
//
//void setup() {
//    Serial.begin(9600);
//    Serial.println("Basic Encoder Test:");
//}
//
//long oldPosition  = -999;
//
//void loop() {
//    long newPosition = myEnc.read();
//    if (newPosition != oldPosition) {
//        oldPosition = newPosition;
//        Serial.println(newPosition);
//    }
//}