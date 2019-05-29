#include <stdint.h>
#include <math.h>

#define SERVO_PIN 2
#define A_SIGNAL 3 //Pin number for encoder
#define B_SIGNAL 4 //Pin munber for encoder
// #define enA 14
#define in1 5
#define in2 6
#define SPEED_CTRL_PIN 23
#define FAILSAFE_PIN 22
#define OP_MODE_PIN 21
#define CTRL_MODE_PIN 20

#define MANUAL_MODE 0
#define AUTO_MODE 1
#define IDLE_MODE 0
#define RELEASE_MODE 1
#define RETRACT_MODE 2

namespace global{
    extern const uint16_t ENCODER_TICK_PER_RES; // ticks.
    extern const uint16_t DRUM_R; // mm
    extern const uint8_t  DRUM_2_ENCODER_GEAR_RATIO_X_10;
    extern const double ENCODER_MM_PER_RES; // 439 ->439.82 / 2
    extern const uint32_t ENCODER_MM_PER_TICK_X_1000;
    extern const uint32_t SERVO_DELAY;


    extern const int LOOP_SPEED ; //ms updating at 1/50*10^3 = 20Hz
    extern const int SPEED_DELTA_T ; //ms
    extern const int RC_DELTA_T ; // ms

    extern const int SERVO_BRAKE ;
    extern const int SERVO_RELEASE ;

    extern const int DESIRED_SPEED ; //1.5m/s, 1500mm/s

    extern const int MOTOR_LOW ;
    extern const int MOTOR_HIGH ;

    extern const long DESIRED_DROP_ALTITUDE ;

    extern const long ALTITUDE_ERROR ;

    extern const int DETTACHMENT_TIME ;

    extern const int UAV_DISPLACMENT ;

    extern const uint16_t RC_CUTOFF_BAND;
};

