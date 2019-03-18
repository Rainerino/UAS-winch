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

#define SIGNAL_PIN 12
#define SERVO_PIN 9
#define A_SIGNAL 2 //Pin number for encoder
#define B_SIGNAL 3 //Pin munber for encoder
#define enA 5
#define in1 4
#define in2 6

const int LOOP_SPEED = 500; //ms updating at 1/50*10^3 = 20Hz
const int SPEED_DELTA_T = 200; //ms
const int RC_DELTA_T = 10;
const int SERVO_BRAKE = 90;
const int SERVO_RELEASE = 0;
const int DESIRED_SPEED = 2000; //2m/s, 2000mm/s

Encoder uas_encoder(2,3);

UAS_driver driver;

void release();
void retract();
void calculate_speed();
void rc_input_update();

bool auto_mission_completed;
bool audo_release_completed;
bool audo_retract_completed;

int rc_reset_channel = 0;
int rc_mode_channel = 0;
int rc_arm_channel = 0;

int prev_mode_channel = 0;



uint16_t difference;
uint16_t altitude = 30000; // goes up to 65 meters

Ticker encoder_speed(calculate_speed, SPEED_DELTA_T); // update speed at certain rate

Ticker rc_update(rc_input_update, RC_DELTA_T); // update rc inputs at certain rate

void setup() {
    driver = UAS_driver();
    driver.attach_motor(enA, in1, in2);
    driver.attach_servo(SERVO_PIN);
    auto_mission_completed = false;
    Serial.begin(9600);
    driver.servo_brake_range(SERVO_RELEASE, SERVO_BRAKE);

    encoder_speed.start();
    rc_update.start();
}

void loop() {
    encoder_speed.update();
    rc_update.update();

    // failsafe

    // check mode: auto or manual.

    if (rc_mode_channel == 0){
        // auto mode
        // when swtich from manual to release, add a reset
        if (prev_mode_channel != rc_mode_channel){
            driver.encoder_reset(uas_encoder);
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
    }else if (rc_mode_channel == 1){

    }



    // manual mode

    Serial.println(ENCODER_MM_PER_TICK_X_1000);
    Serial.println(ENCODER_MM_PER_RES);
    Serial.println(ENCODER_TICK_PER_RES);

    Serial.print("TOTAL Ticks: ");
    Serial.println(driver.encoder_cur_tick);

    Serial.print("Distance: ");
    Serial.println(driver.encoder_total_distance(uas_encoder));

    Serial.print("Difference: ");
    Serial.println(difference);

    Serial.print("Speed: ");
    Serial.println(driver.current_speed);

    Serial.print("Valid? ");
    Serial.println(driver.encoder_invalid);

    Serial.print("RAW READ ");
    Serial.println(uas_encoder.read());
    Serial.println("============================");
    delay(LOOP_SPEED);
}


void release(){
    if(driver.encoder_total_distance(uas_encoder) < altitude){
        driver.servo_release();
        // speed controller goes here.
        // a simple pid controller

        int prev_err;
        int err = DESIRED_SPEED - driver.current_speed;
        int I_err = err + I_err;
        int D_err = err - prev_err;

        int Kp, KD, Ki;
        int change = Kp * err + KD * D_err + Ki * I_err;
        prev_err = err;

        



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
}

