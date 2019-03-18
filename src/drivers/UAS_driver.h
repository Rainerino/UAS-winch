//
// Created by yiyi on 17/03/19.
//

#ifndef UAS_WINCH_UAS_DRIVER_H
#define UAS_WINCH_UAS_DRIVER_H


#include <Servo.h>
#include <Encoder.h>
#include <main.h>
#include "math.h"


const uint16_t ENCODER_TICK_PER_RES = 81; // ticks.
const uint16_t DRUM_R = 35; // mm
const uint8_t  DRUM_2_ENCODER_GEAR_RATIO_X_10 = 14;
const double ENCODER_MM_PER_RES  = 2* M_PI * DRUM_R; // 439 ->439.82 / 2
const uint32_t ENCODER_MM_PER_TICK_X_1000 =  uint32_t((1000* ENCODER_MM_PER_RES / ENCODER_TICK_PER_RES) *
        DRUM_2_ENCODER_GEAR_RATIO_X_10/10) ; // 565-> 7587->7601/2, should be 542.9913 * 1.4 = 7.606 -> 7606/2



class UAS_driver {

public:
    // set up funtions
    UAS_driver();
    void attach_motor(uint8_t motor_pin,uint8_t in1, uint8_t in2);
    void attach_servo(int servo_pin);

    void encoder_reset(Encoder encoder);
    uint32_t encoder_total_distance(Encoder uas_encoder);
    bool encoder_valid(uint16_t delta_t);
    uint16_t encoder_tick_diff(Encoder uas_encoder);



    void motor_set_range(uint16_t low, uint16_t high);
    void motor_stop();
    void motor_start();
    void motor_run_at(float percent);
    void motor_reverse_direction();

    void servo_brake_range(uint16_t low, uint16_t high);
    void servo_brake_at(float brake_precent);
    void servo_release();
    void servo_full_brake();
    void servo_slow_brake();

    void test_message();

    uint16_t current_speed;
    uint16_t encoder_max_rpm;
    uint32_t encoder_cur_tick;
    bool encoder_invalid;

private:

    uint32_t encoder_prev_tick;


    uint16_t motor_low;
    uint16_t motor_high;
    uint8_t motor_pin;
    uint8_t motor_in1;
    uint8_t motor_in2;

    bool motor_direction;

    uint16_t servo_low_brake_angle;
    uint16_t servo_high_brake_angle; // this is the full brake angle

    Servo uas_servo;
//    Encoder uas_encoder;

};


#endif //UAS_WINCH_UAS_DRIVER_H
