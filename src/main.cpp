//
// Created by yiyi on 17/03/19.
//

/*
 * Driver module
 */
#include "main.h"
#include "Arduino.h"
#include "drivers/UAS_driver.h"

#define SIGNAL_PIN 12
#define SERVO_PIN 9
#define A_SIGNAL 6 //Pin number for encoder
#define B_SIGNAL 7 //Pin munber for encoder
#define enA 5
#define in1 4
#define in2 3

#define LOOP_SPEED 50

UAS_driver driver;

void setup() {
    driver = UAS_driver();
    driver.attach_encoder(A_SIGNAL, B_SIGNAL);
    driver.attach_motor(enA, in1, in2);
    driver.attach_servo(SERVO_PIN);
}

void loop() {
    // failsafe

    //

}

void start_release(){

}
void start_retract(){

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