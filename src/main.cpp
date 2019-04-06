//
// Created by yiyi on 17/03/19.
//

#include <Ticker.h>
#include "Arduino.h"
#include "UAS_drivers/UAS_driver.h"

#define SERVO_PIN 7
#define A_SIGNAL 8 //Pin number for encoder
#define B_SIGNAL 9 //Pin munber for encoder
#define enA 14
#define in1 10
#define in2 11
#define SPEED_CTRL_PIN 23
#define FAILSAFE_PIN 22
#define OP_MODE_PIN 21
#define CTRL_MODE_PIN 20

#define MANUAL_MODE 0
#define AUTO_MODE 1
#define IDLE_MODE 0
#define RELEASE_MODE 1
#define RETRACT_MODE 2

/*
 * Data type related constants
 */
const int LOOP_SPEED = 50; //ms updating at 1/50*10^3 = 20Hz
const int SPEED_DELTA_T = 200; //ms
const int RC_DELTA_T = 10; // ms

const int SERVO_BRAKE = 95;
const int SERVO_RELEASE = 40;

const int DESIRED_SPEED = 1500; //1.5m/s, 1500mm/s
const int MOTOR_LOW = 100;
const int MOTOR_HIGH = 255;

const long DESIRED_DROP_ALTITUDE = 0.5;




Encoder uas_encoder(A_SIGNAL,B_SIGNAL);

UAS_driver driver;

void release();
void retract();
void calculate_speed();
void rc_input_update();

bool auto_mission_completed;
bool auto_release_completed;
bool audo_retract_completed;



uint16_t difference;

uint16_t current_altitude = 30000; // goes up to 65 meters PLACE HOLDER
uint16_t drone_altitude = 30000; // PLACE HOLDER


Ticker encoder_speed(calculate_speed, SPEED_DELTA_T, 0); // update speed at certain rate

Ticker rc_update(rc_input_update, RC_DELTA_T, 0); // update rc inputs at certain rate


void release(){
    if(current_altitude <= DESIRED_DROP_ALTITUDE){
        driver.servo_full_brake();
        driver.encoder_reset(uas_encoder);
        auto_release_completed = true;
    } else {
        driver.servo_brake_at(100* (1-log(current_altitude + DESIRED_DROP_ALTITUDE)/log(30+DESIRED_DROP_ALTITUDE) * driver.current_speed/DESIRED_SPEED));
    }   
}

void retract(){

    if(current_altitude < drone_altitude && (current_altitude < drone_altitude-3 || driver.current_speed == 0)){
        driver.servo_release();
        driver.motor_run_at(100* (1- log(current_altitude)/log(drone_altitude)));
    } else {
        driver.servo_full_brake();
        driver.encoder_reset(uas_encoder);
        auto_mission_completed = true;
    }
}

/**
 * Interrupt function 1: update the speed measurement
 */
void calculate_speed(){
    driver.encoder_update_current_speed(SPEED_DELTA_T, uas_encoder);
    // Give out warning if the current speed exceeds the rated speed, that means encoder is likely to be
    // missing steps and requires some restart or manual control.
    driver.encoder_valid(SPEED_DELTA_T);
}

/**
 * Interrupt function 2: update rc inputs
 */
void rc_input_update(){
    //noInterrupts();

    // update operation mode
    driver.rc_op_mode.raw_value  =  pulseIn(driver.rc_op_mode.pin, HIGH);
    driver.rc_op_mode.change = (driver.rc_op_mode.raw_value > 1500) ^ (driver.rc_op_mode.mode == 1); // xor, only if there is a mode change
    driver.rc_op_mode.mode = uint8_t(driver.rc_op_mode.raw_value > 1500 ? AUTO_MODE : MANUAL_MODE);

    // update failsafe control data
    driver.rc_failsafe.raw_value = pulseIn(driver.rc_failsafe.pin, HIGH);
    driver.rc_failsafe.trigger = driver.rc_failsafe.raw_value > 1500;

    driver.rc_speed_ctrl.raw_value = pulseIn(driver.rc_speed_ctrl.pin, HIGH);
    driver.rc_speed_ctrl.percentage = uint16_t(map(driver.rc_speed_ctrl.raw_value, 950, 2000, 0, 100));
    //    Serial.println("CALLED");
    driver.rc_ctrl_mode.raw_value = pulseIn(driver.rc_ctrl_mode.pin, HIGH);

    if(driver.rc_ctrl_mode.raw_value < 1200){ //
        driver.rc_ctrl_mode.mode = IDLE_MODE;
    }else if(driver.rc_ctrl_mode.raw_value > 1700){
        driver.rc_ctrl_mode.mode = RELEASE_MODE;
    }else{
        driver.rc_ctrl_mode.mode = RETRACT_MODE; // mid is retract
    }
//    interrupts();

}

/**
 * this is the compeition operation set up function. We seperated it so that testing codes are easier to add.
 */
void static main_operation_setup(){
    driver = UAS_driver();
    driver.attach_motor(enA, in1, in2);
    driver.attach_servo(SERVO_PIN);
    auto_mission_completed = false;
    
    Serial.begin(9600);
    driver.servo_brake_range(SERVO_RELEASE, SERVO_BRAKE);
    driver.rc_ctrl_mode.pin = CTRL_MODE_PIN;
    driver.rc_op_mode.pin = OP_MODE_PIN;
    driver.rc_speed_ctrl.pin = SPEED_CTRL_PIN;
    driver.rc_failsafe.pin = FAILSAFE_PIN;
    driver.motor_set_range(MOTOR_LOW, MOTOR_HIGH);

    //set up input and output pins
    driver.setup_pinMode();
    driver.lcd_setup();
    
    encoder_speed.start();
    rc_update.start();
}

/**
 * this is the compeition operation loop.
 */
void static main_operation_loop() {

    encoder_speed.update();
    rc_update.update();

    // if (driver.rc_op_mode.mode == AUTO_MODE){
    if (false){
        Serial.println("AUTO MODE IS NOT COMPLETED");
        // auto mode
        // when swtich from manual to release, add a reset
        if (driver.rc_op_mode.change){
            driver.encoder_reset(uas_encoder);
            driver.rc_op_mode.change = false;
        }

        if (!auto_mission_completed){
            if(!auto_release_completed) {
                release();
            }else if (!audo_retract_completed){
                retract();
            }else{
                // clean up function. mission is completed, we need to go to the idle mode
            }
            auto_mission_completed = true;
        }
        // manual mode
    // }else if (driver.rc_op_mode.mode == AUTO_MODE){
     }else if (true){   

        if (driver.rc_failsafe.trigger){
            driver.servo_full_brake();
            driver.motor_stop();
        }else{
            if(driver.rc_ctrl_mode.mode == RELEASE_MODE){
                driver.servo_brake_at(driver.rc_speed_ctrl.percentage);
                driver.motor_stop();
            }else if(driver.rc_ctrl_mode.mode == RETRACT_MODE){
                driver.servo_release();
//                driver.motor_start();
                  driver.motor_run_at(driver.rc_speed_ctrl.percentage);
            }else{
                driver.servo_slow_brake();
                driver.motor_stop();
            }
        }
    }

    driver.driver_test_message(uas_encoder);
    driver.lcd_display_encoder_data(uas_encoder);
    delay(LOOP_SPEED);
}

void setup() {
    main_operation_setup();
}

void loop() {

    main_operation_loop();

}

