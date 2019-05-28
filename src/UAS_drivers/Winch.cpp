#include "Winch.h"
using namespace winch;

Winch::Winch(){
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
    auto_dettachment_completed = false;
    auto_retract_completed = false;

    driver.motor_set_range(MOTOR_LOW, MOTOR_HIGH);

    //set up input and output pins
    driver.setup_pinMode();

}
void Winch::updateRCInput(){
    this->
}

void Winch::updateRCTriggerInput(){
    driver.rc_failsafe.raw_value = pulseIn(driver.rc_failsafe.pin, HIGH);
    driver.rc_failsafe.trigger = driver.rc_failsafe.raw_value > 1500;
}
void Winch::updateRCModeInput(){
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
}

void Winch::autoMode(){

}
void Winch::manualMode(){

}

void Winch::release(){

}
void Winch::retract(){

}

void Winch::releaseController(){

}



