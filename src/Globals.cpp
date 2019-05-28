#include "Globals.h"

uint16_t ENCODER_TICK_PER_RES = 81; // ticks.
uint16_t DRUM_R = 35; // mm
const uint8_t  DRUM_2_ENCODER_GEAR_RATIO_X_10 = 14;
const double ENCODER_MM_PER_RES  = 2* M_PI * DRUM_R; // 439 ->439.82 / 2
const uint32_t ENCODER_MM_PER_TICK_X_1000 =  uint32_t((1000* ENCODER_MM_PER_RES / ENCODER_TICK_PER_RES) *
        DRUM_2_ENCODER_GEAR_RATIO_X_10/10) ; // 565-> 7587->7601/2, should be 542.9913 * 1.4 = 7.606 -> 7606/2

const uint32_t SERVO_DELAY = 10;


const int LOOP_SPEED = 50; //ms updating at 1/50*10^3 = 20Hz
const int SPEED_DELTA_T = 200; //ms
const int RC_DELTA_T = 10; // ms

const int SERVO_BRAKE = 95;
const int SERVO_RELEASE = 40;

const int DESIRED_SPEED = 1500; //1.5m/s, 1500mm/s

const int MOTOR_LOW = 100;
const int MOTOR_HIGH = 255;

const long DESIRED_DROP_ALTITUDE = 3000;

const long ALTITUDE_ERROR = 0.1;

const int DETTACHMENT_TIME = 5000;

const int UAV_DISPLACMENT = 200;