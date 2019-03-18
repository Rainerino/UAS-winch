//
// Created by yiyi on 17/03/19.
//

#ifndef UAS_WINCH_UAS_DRIVER_H
#define UAS_WINCH_UAS_DRIVER_H


#include <Servo.h>
#include <Encoder.h>
#include <main.h>
const uint8_t ENCODER_TICK_2_DEG = uint8_t (45*1.5); // degree
const uint8_t DRUM_R = 70; // mm
const uint8_t ENCODER_DEG_2_MM  = uint8_t(2* M_PI * DRUM_R * ENCODER_TICK_2_DEG / 360);

class UAS_driver {

public:
    // set up funtions
    UAS_driver();
    void attach_encoder(uint8_t encoder_pin_1, uint8_t encoder_pin_2);
    void attach_motor(uint8_t motor_pin,uint8_t in1, uint8_t in2);
    void attach_servo(int servo_pin);


    void encoder_reset();
    unsigned long encoder_total_distance();
    bool encoder_valid();
    void encoder_current_speed();
    void encoder_set_max_rpm(int max_rpm);


    void motor_set_range(int low, int high);
    void motor_stop();
    void motor_start();
    void motor_run_at(float percent);
    void motor_reverse_direction();

    void servo_set_stop(int stop_angle);
    void servo_brake_range(int low, int high);
    void servo_set_release(int release_angle);
    void servo_brake_at(float brake_precent);
    void servo_release();
    void servo_full_brake();

private:
    unsigned long total_distance;

    unsigned long encoder_total_tick;

    unsigned int motor_low;
    unsigned int motor_high;

    unsigned int servo_low_brake_angle;
    unsigned int servo_high_brake_angle; // this is the full brake angle
    unsigned int servo_release_angle;

    Servo uas_servo;
    Encoder uas_encoder = Encoder(0, 0);

    bool encoder_invalid;
};


#endif //UAS_WINCH_UAS_DRIVER_H
