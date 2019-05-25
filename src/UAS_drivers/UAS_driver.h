//
// Created by yiyi on 17/03/19.
//

#ifndef UAS_WINCH_UAS_DRIVER_H
#define UAS_WINCH_UAS_DRIVER_H



#include <PWMServo.h>
#include <Encoder.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <math.h>

#include "../Globals.h"

#undef in1
#undef in2


class UAS_driver {

public:
    // set up funtions
    struct rc_channel{
        uint8_t pin;
        uint32_t raw_value; // just in case
        bool trigger; // save the trigger value. Tru eto triggered
        uint8_t mode; // to save mode
        bool change; // for detecting mode changes
        uint16_t percentage; // value out of 100, for speed control
    }rc_failsafe, rc_speed_ctrl, rc_op_mode, rc_ctrl_mode;

    /** =======================================================
     * Set up functions
     * ========================================================
     */

    UAS_driver();


    void setup_pinMode();

    /**
     * Set up and change the motor pins.
     *
     * @param motor_pin
     * @param in1
     * @param in2
     */
    void attach_motor(uint8_t in1, uint8_t in2);

    /**
     * Set up and attach the servo pin. Can be change at anytime.
     * @param servo_pin
     */
    void attach_servo(int servo_pin);

    /**
     * Reset the encoder data. Start the tick from 0.
     * @param encoder
     */
    void encoder_reset(Encoder encoder);

    /**
     * Set up the range for servo controll
     * @param low
     * @param high
     */
    void servo_brake_range(uint16_t low, uint16_t high);



    /**=======================================================
    * Encoder functions
    * ========================================================
    */

    /**
    *
    * @param uas_encoder
    * @return
    */
    uint32_t encoder_total_distance(Encoder uas_encoder);

    /**
    *
    * @param uas_encoder
    * @return
    */
    int encoder_distance(Encoder uas_encoder);

    /**
     *
     * @param delta_t
     * @return
     */
    bool encoder_valid(uint16_t delta_t);

    /**
     *
     * @param uas_encoder
     * @return
     */
    int32_t encoder_tick_diff(Encoder uas_encoder);

    /**
     * Update the current speed based on delta_t
     * @param delta_t
     * @return
     */
    void encoder_update_current_speed(uint16_t delta_t, Encoder uas_encoder);

    /**
     * Update the altitude of the UAV
     * @return
     */
    int16_t update_UAV_altitude();                          //NEEDS TO BE WRITTEN

    /**=======================================================
    * Motor functions
    * ========================================================
    */

    /**
     * Set the motor control range. This is for speed control, therefore low should be the lowest functional speed
     * instead of completely stopping
     * @param low
     * @param high
     */
    void motor_set_range(uint16_t low, uint16_t high);

    /**
     * Complete stop the motor motion
     */
    void motor_stop();

    /**
     * Currently set motor to its max speed
     */
    void motor_start();

    /**
     *
     * @param percent
     */
    void motor_run_at(uint16_t percent);

    /**=======================================================
    * Servo functions
    * ========================================================
    */

    /**
     *
     * @param brake_percent
     */
    void servo_brake_at(uint16_t brake_percent);

    /**
     *
     */
    void servo_release();

    /**
     *
     */
    void servo_full_brake();

    void servo_slow_brake();


    /**=======================================================
    * Testing functions
    * ========================================================
    */
    /**
     * Display informative testing messages to the serial monitor
     * @param uas_encoder
     */
    void driver_test_message(Encoder uas_encoder);


    /**=======================================================
    * LCD functions
    * ========================================================
    */

    void lcd_setup();
    void lcd_display_message(char* message);
    void lcd_display_encoder_data(Encoder uas_encoder);

    /* ==================================  Drafting problem */

    void motor_reverse_direction();

    void change_speed();
    /* ================================== */

    uint16_t current_speed;
    uint16_t encoder_max_rpm;
    uint32_t encoder_cur_tick;
    int encoder_cur_tick_int;
    bool encoder_invalid;
    uint32_t encoder_prev_tick;

private:
    
    uint16_t motor_low;
    uint16_t motor_high;
    uint8_t motor_in1;
    uint8_t motor_in2;
    uint8_t motor_pin;

    bool motor_direction;

    uint16_t servo_low_brake_angle;
    uint16_t servo_high_brake_angle; // this is the full brake angle

    PWMServo uas_servo;
};


#endif //UAS_WINCH_UAS_DRIVER_H
