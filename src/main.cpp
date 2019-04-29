//
// Created by yiyi on 17/03/19.
//

#include <Ticker.h>
#include "Arduino.h"
#include "UAS_drivers/UAS_driver.h"

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

const long ALTITUDE_ERROR = 0.1;

const int DEATTACHMENT_TIME = 5000;

const int UAV_DISPLACMENT = 200;

Encoder uas_encoder(A_SIGNAL,B_SIGNAL);

UAS_driver driver =  UAS_driver();

void release();
void retract();
void calculate_speed();
void rc_input_update();

bool auto_mission_completed;
bool auto_release_completed;
bool auto_deattachment_completed;
bool auto_retract_completed;

uint16_t current_altitude;
uint16_t UAV_altitude;


Ticker encoder_speed(calculate_speed, SPEED_DELTA_T, 0); // update speed at certain rate

Ticker rc_update(rc_input_update, RC_DELTA_T, 0); // update rc inputs at certain rate



/**
 * Realeasing the rope and controlling its speed to lower the UGV to the DESIRED_DROP_ALTITUDE.
 */
void release(){
    if(current_altitude <= DESIRED_DROP_ALTITUDE){
        driver.servo_full_brake();
        driver.encoder_reset(uas_encoder);
        auto_release_completed = true;
    } else {
        //100 because using percentage
        //+1 beacue log(<1) is negative
        driver.servo_brake_at(100 * (1 - log(current_altitude - DESIRED_DROP_ALTITUDE+1) / log(30-DESIRED_DROP_ALTITUDE+1)));
    }   
}

/**
 * Waiting for the UGV to de-attach.
 */
void deattach(){
    delay(DEATTACHMENT_TIME); //Wait Time
    auto_deattachment_completed = true;
}

/**
 * Retract the rope after the UGV is de-attached.
 */
void retract(){

    //Two conditions for retracting the rope:
    //Current altitude is less than the UAV altitude wirth error
    //The rope is really close to the UAV or the rope already reached the drum and can not move (currrent speed = 0)
    if(current_altitude < (UAV_altitude*(1-ALTITUDE_ERROR)-UAV_DISPLACMENT) ||
    !(current_altitude > UAV_altitude*(1-ALTITUDE_ERROR)-UAV_DISPLACMENT && driver.current_speed == 0)){
        driver.servo_release();
        //altitude +1 because log(<1) is negative
        driver.motor_run_at(100* (1- log(current_altitude+1)/log(UAV_altitude+1)));
    } else {
        driver.servo_full_brake();
        driver.encoder_reset(uas_encoder);
        auto_retract_completed = true;
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
 * Update altitude of the rover
 */
void update_current_altitude(){
    current_altitude = UAV_altitude - UAV_DISPLACMENT + driver.encoder_distance(uas_encoder);
}

/**
 * Interrupt function 2: update rc inputs
 */
void rc_input_update(){                         //NEEDS TO BE UPDATED WITH DE-ATTACHMENT PHASE -Yekta
    //noInterrupts();
    // update operation mode
    driver.rc_op_mode.raw_value  =  pulseIn(driver.rc_op_mode.pin, HIGH);
    driver.rc_op_mode.change = driver.rc_op_mode.raw_value > 1700 ; // xor, only if there is a mode change
    // driver.rc_op_mode.mode = uint8_t(driver.rc_op_mode.raw_value > 1500 ? AUTO_MODE : MANUAL_MODE);
    if (driver.rc_op_mode.change){
        driver.rc_op_mode.mode = driver.rc_op_mode.mode == AUTO_MODE ? MANUAL_MODE : AUTO_MODE;
    }
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
    driver.attach_motor(in1, in2);
    driver.attach_servo(SERVO_PIN);
    auto_mission_completed = false;
    
    Serial.begin(9600);
    driver.servo_brake_range(SERVO_RELEASE, SERVO_BRAKE);
    driver.rc_ctrl_mode.pin = CTRL_MODE_PIN;
    driver.rc_op_mode.pin = OP_MODE_PIN;
    driver.rc_speed_ctrl.pin = SPEED_CTRL_PIN;
    driver.rc_failsafe.pin = FAILSAFE_PIN;

    auto_mission_completed = false;
    auto_release_completed = false;
    auto_deattachment_completed = false;
    auto_retract_completed = false;

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
    UAV_altitude = driver.update_UAV_altitude();
    update_current_altitude();

    if (driver.rc_op_mode.mode == AUTO_MODE){

        Serial.println("AUTO MODE IS NOT COMPLETED");
        // auto mode
        // when swtich from manual to release, add a reset
        if (driver.rc_op_mode.change){
            driver.encoder_reset(uas_encoder);
            driver.rc_op_mode.change = false;
        }else if (!auto_mission_completed){
            if(!auto_release_completed) {
                release();
            }else if (!auto_deattachment_completed){
                deattach();
            }else if (!auto_retract_completed){
                retract();
            }else{
                // clean up function. mission is completed, we need to go to the idle mode
            }
            auto_mission_completed = true;
        }
    //     manual mode
    }else if (driver.rc_op_mode.mode == MANUAL_MODE){             //NEEDS TO BE UPDATED WITH DE-ATTACHMENT PHASE

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

