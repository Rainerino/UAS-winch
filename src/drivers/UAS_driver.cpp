//
// Created by yiyi on 17/03/19.
//

#include "UAS_driver.h"

UAS_driver::UAS_driver(){
    encoder_invalid = false;
    total_distance = 0;

}
void UAS_driver::attach_encoder(uint8_t encoder_pin_1, uint8_t encoder_pin_2){
    uas_encoder = Encoder(encoder_pin_1, encoder_pin_2);
}
void UAS_driver::attach_motor(uint8_t motor_pin, uint8_t in1, uint8_t in2){
    pinMode(motor_pin, OUTPUT);
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    digitalWrite(in1, HIGH); //LOW if releasing the rope, HIGH if realing the rope
    digitalWrite(in2, LOW); //HIGH if releasing the rope, LOW if realing the rope

}
void UAS_driver::attach_servo(int servo_pin){
    uas_servo.attach(servo_pin);
}


void UAS_driver::encoder_reset(){

}
unsigned long UAS_driver::encoder_total_distance(){

    return uint32_t(encoder_total_tick / ENCODER_DEG_2_MM);
}

bool UAS_driver::encoder_valid(){

}
void UAS_driver::encoder_current_speed(){

}
void UAS_driver::encoder_set_max_rpm(int max_rpm){

}


void UAS_driver::motor_set_range(int low, int high){

}
void UAS_driver::motor_stop(){

}
void UAS_driver::motor_start(){

}
void UAS_driver::motor_run_at(float percent){

}
void UAS_driver::motor_reverse_direction(){


}

void UAS_driver::servo_set_stop(int stop_angle){

}
void UAS_driver::servo_brake_range(int low, int high){

}
void UAS_driver::servo_set_release(int release_angle){

}
void UAS_driver::servo_brake_at(float brake_precent){

}
void UAS_driver::servo_release(){

}
void UAS_driver::servo_full_brake(){

}

