//
// Created by yiyi on 17/03/19.
//

/*
 * Driver module
 */
#include <Ticker.h>
#include "main.h"
#include "Arduino.h"
#include "drivers/UAS_driver.h"


#define SERVO_PIN 9
#define A_SIGNAL 2 //Pin number for encoder
#define B_SIGNAL 3 //Pin munber for encoder
#define enA 5
#define in1 4
#define in2 6
#define SPEED_CTRL_PIN 4
#define FAILSAFE_PIN A2
#define OP_MODE_PIN A3
#define CTRL_MODE_PIN A4

#define MANUAL_MODE 0
#define AUTO_MODE 1
#define IDLE_MODE 0
#define RELEASE_MODE 1
#define RETRACT_MODE 2

const int LOOP_SPEED = 500; //ms updating at 1/50*10^3 = 20Hz
const int SPEED_DELTA_T = 200; //ms
const int RC_DELTA_T = 10; // ms
const int SERVO_BRAKE = 90;
const int SERVO_RELEASE = 0;
const int DESIRED_SPEED = 2000; //2m/s, 2000mm/s

Encoder uas_encoder(A_SIGNAL,B_SIGNAL);

UAS_driver driver;

void release();
void retract();
void calculate_speed();
void rc_input_update();

bool auto_mission_completed;
bool audo_release_completed;
bool audo_retract_completed;

int prev_mode_channel = 0;



uint16_t difference;
uint16_t altitude = 30000; // goes up to 65 meters

Ticker encoder_speed(calculate_speed, SPEED_DELTA_T); // update speed at certain rate

Ticker rc_update(rc_input_update, RC_DELTA_T, 0); // update rc inputs at certain rate



void release(){
    if(driver.encoder_total_distance(uas_encoder) < altitude){
        driver.servo_release();
        // speed controller goes here.
        // a simple pid controller
    } else{
        driver.servo_full_brake();
        driver.encoder_reset(uas_encoder);
        audo_release_completed = true;
    }
}
void retract(){
    //TODO
}

void calculate_speed(){
    difference = driver.encoder_tick_diff(uas_encoder);

    if (difference!= 0) {
        driver.current_speed = uint16_t((difference * ENCODER_MM_PER_TICK_X_1000 / 1000.0) * (1000 / SPEED_DELTA_T)); // mm/s
    }else{
        // avoid 0 division
        driver.current_speed = 0;
    }
    driver.encoder_valid(SPEED_DELTA_T);
}
void rc_input_update(){
    // channel updates
    // update op_mode change
    driver.rc_op_mode.raw_value  =  pulseIn(driver.rc_op_mode.pin, HIGH);
    driver.rc_op_mode.change = (driver.rc_op_mode.raw_value > 1500) ^ (driver.rc_op_mode.mode == 1); // xor, only if there is a mode change
    driver.rc_op_mode.mode = uint8_t(driver.rc_op_mode.raw_value > 1500 ? AUTO_MODE : MANUAL_MODE);

    driver.rc_failsafe.raw_value = pulseIn(driver.rc_failsafe.pin, HIGH);
    driver.rc_failsafe.trigger = driver.rc_failsafe.raw_value > 1500;

    driver.rc_speed_ctrl.raw_value = pulseIn(driver.rc_speed_ctrl.pin, HIGH);
    driver.rc_speed_ctrl.precentage = uint16_t(map(driver.rc_speed_ctrl.raw_value, 1000, 2000, 0, 100));
    driver.rc_ctrl_mode.raw_value = pulseIn(driver.rc_ctrl_mode.pin, HIGH);

}

void static test_rc_setup(){
    driver.attach_servo(SERVO_PIN);
    driver.servo_brake_range(SERVO_RELEASE, SERVO_BRAKE);
    driver.rc_speed_ctrl.pin = SPEED_CTRL_PIN;
    Serial.begin(9600);
    rc_update.start();
}
void static test_rc_loop() {
    rc_update.update();
    driver.servo_brake_at(driver.rc_speed_ctrl.precentage);

//    Serial.println(driver.rc_speed_ctrl.precentage);
//    Serial.println(driver.rc_speed_ctrl.raw_value);
//    Serial.println(pulseIn(driver.rc_speed_ctrl.pin, HIGH));
//    Serial.println("=============");
}

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
    encoder_speed.start();
    rc_update.start();
}
void static main_operation_loop() {
    encoder_speed.update();
    rc_update.update();

    // failsafe

    // check mode: auto or manual.

    if (driver.rc_op_mode.mode == AUTO_MODE){
        // auto mode
        // when swtich from manual to release, add a reset
        if (driver.rc_op_mode.change){
            driver.encoder_reset(uas_encoder);
            driver.rc_op_mode.change = false;
        }

        if (!auto_mission_completed){
            if(!audo_release_completed) {
                release();
            }else if (!audo_retract_completed){
                retract();
            }else{
                // clean up function. mission is completed, we need to go to the idle mode
            }
            auto_mission_completed = true;
        }
        // manual mode
    }else if (driver.rc_op_mode.mode == MANUAL_MODE){

    }

    driver.driver_test_message(uas_encoder);
    delay(LOOP_SPEED);
}

void setup() {
    test_rc_setup();
}

void loop() {
    test_rc_loop();

}

