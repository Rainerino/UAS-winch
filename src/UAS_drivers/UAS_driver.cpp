//
// Created by yiyi on 17/03/19.
//


#include "./UAS_driver.h"
#include "UAS_driver.h"

#define OLED_ADDR   0x3C
Adafruit_SSD1306 display(-1);

UAS_driver::UAS_driver() {
    encoder_invalid = false;
    encoder_max_rpm = 450;
    motor_direction = true;
    encoder_prev_tick = 0;
    encoder_cur_tick = 0;
    current_speed = 0;

}
void UAS_driver::setup_pinMode(){
    pinMode(motor_pin, OUTPUT);
    pinMode(motor_in1, OUTPUT);
    pinMode(motor_in2, OUTPUT);
    pinMode(rc_op_mode.pin, INPUT);
    pinMode(rc_failsafe.pin, INPUT);
    pinMode(rc_speed_ctrl.pin, INPUT);
    pinMode(rc_ctrl_mode.pin, INPUT);
}

void UAS_driver::attach_motor(uint8_t pin, uint8_t in1, uint8_t in2){
    motor_pin = pin;
    motor_in1 = in1;
    motor_in2 = in2;

    pinMode(pin, OUTPUT);
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    if (motor_direction){
        digitalWrite(in1, HIGH); //LOW if releasing the rope, HIGH if realing the rope
        digitalWrite(in2, LOW); //HIGH if releasing the rope, LOW if realing the rope
    }else{
        digitalWrite(in1, LOW); //LOW if releasing the rope, HIGH if realing the rope
        digitalWrite(in2, HIGH); //HIGH if releasing the rope, LOW if realing the rope
    }

}
void UAS_driver::attach_servo(int servo_pin){
    uas_servo.attach(servo_pin);
}

void UAS_driver::encoder_reset(Encoder uas_encoder){
    uas_encoder.write(0);
    encoder_cur_tick = uint32_t(abs(uas_encoder.read()));
}
uint32_t UAS_driver::encoder_total_distance(Encoder uas_encoder){
    encoder_cur_tick = uint32_t(abs(uas_encoder.read()));
    return uint32_t(encoder_cur_tick * ENCODER_MM_PER_TICK_X_1000 / 1000.0);
}

bool UAS_driver::encoder_valid(uint16_t delta_t){
    // rpm -> rps -> tps -> mm ps -> mmps * s/s
    encoder_invalid = current_speed > (((encoder_max_rpm / 60) * ENCODER_TICK_PER_RES) * (ENCODER_MM_PER_TICK_X_1000 /1000.0) ) * (delta_t/1);
    return encoder_invalid;
}

void UAS_driver::encoder_update_current_speed(uint16_t delta_t, Encoder uas_encoder){
    auto difference = encoder_tick_diff(uas_encoder);

    if (difference!= 0) {
        current_speed = uint16_t((difference * ENCODER_MM_PER_TICK_X_1000 / 1000.0) * (1000 / delta_t)); // mm/s
    }else{
        // avoid 0 division
        current_speed = 0;
    }
}


uint16_t UAS_driver::encoder_tick_diff(Encoder uas_encoder){
    encoder_cur_tick = uint32_t(abs(uas_encoder.read()));
    auto difference = uint16_t(abs(encoder_cur_tick - encoder_prev_tick));
    encoder_prev_tick = encoder_cur_tick;
    return difference;
}

void UAS_driver::motor_set_range(uint16_t low, uint16_t high){
    motor_high = high;
    motor_low = low;
    digitalWrite(motor_in1, HIGH); //LOW if releasing the rope, HIGH if realing the rope
    digitalWrite(motor_in2, LOW); //HIGH if releasing the rope, LOW if realing the rope
}
void UAS_driver::motor_stop(){
    analogWrite( motor_pin, 0);
}
void UAS_driver::motor_start(){
    analogWrite(motor_pin, uint8_t(motor_high));
}
void UAS_driver::motor_run_at(float percent){
    analogWrite(motor_pin, uint8_t( (motor_high - motor_low) * percent ));
}
void UAS_driver::motor_reverse_direction(){
    motor_direction = !motor_direction;
    if (motor_direction){
        digitalWrite(motor_in1, HIGH); //LOW if releasing the rope, HIGH if realing the rope
        digitalWrite(motor_in2, LOW); //HIGH if releasing the rope, LOW if realing the rope
    }else{
        digitalWrite(motor_in2, LOW); //LOW if releasing the rope, HIGH if realing the rope
        digitalWrite(motor_in1, HIGH); //HIGH if releasing the rope, LOW if realing the rope
    }
}

// ========================================================================
void UAS_driver::servo_brake_range(uint16_t low, uint16_t high){
    servo_low_brake_angle = low;
    servo_high_brake_angle = high;
}
void UAS_driver::servo_brake_at(uint16_t brake_precent){
    uas_servo.write(static_cast<int>(map(brake_precent , 0, 100, servo_low_brake_angle, servo_high_brake_angle)));
    delayMicroseconds(SERVO_DELAY);
}
void UAS_driver::servo_release(){
    uas_servo.write(servo_low_brake_angle);
    delayMicroseconds(SERVO_DELAY);// for servo to get to that position;
}
void UAS_driver::servo_full_brake(){
    uas_servo.write(servo_high_brake_angle);
    delayMicroseconds(SERVO_DELAY);// for servo to get to that position;
}

void UAS_driver::servo_slow_brake() {
    uas_servo.write((servo_high_brake_angle-servo_low_brake_angle)/2);
    delayMicroseconds(SERVO_DELAY);// for servo to get to that position;
}

void UAS_driver::driver_test_message(Encoder uas_encoder){
    // manual mode
    Serial.println("============================");
    Serial.println(ENCODER_MM_PER_TICK_X_1000);
    Serial.println(ENCODER_MM_PER_RES);
    Serial.println(ENCODER_TICK_PER_RES);

    Serial.print("TOTAL Ticks: ");
    Serial.println(encoder_cur_tick);

    Serial.print("Distance: ");
    Serial.println(this->encoder_total_distance(uas_encoder));

    Serial.print("Difference: ");
    Serial.println(encoder_cur_tick - encoder_prev_tick);

    Serial.print("Speed: ");
    Serial.println(current_speed);

    Serial.print("Valid? ");
    Serial.println(encoder_invalid);

    Serial.print("RAW_Encoder_reading: ");
    Serial.println(uas_encoder.read());
    Serial.print("FAILSAFE: ");
    Serial.println(rc_failsafe.raw_value);
    Serial.print("Speed: ");
    Serial.println(rc_speed_ctrl.raw_value);
    Serial.print("control: ");
    Serial.println(rc_ctrl_mode.raw_value);
    Serial.print("op: ");
    Serial.println(rc_op_mode.raw_value);

    Serial.println("============================");
    Serial.println();
}
void UAS_driver::lcd_setup() {

  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.display();

  // display a pixel in each corner of the screen
  display.drawPixel(0, 0, WHITE);
  display.drawPixel(127, 0, WHITE);
  display.drawPixel(0, 31, WHITE);
  display.drawPixel(127, 31, WHITE);

  // display a line of text
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(27,15);
  display.print("Hello, world!");

  // update display with all of the above graphics
  display.display();
}
void UAS_driver::lcd_display_message(char *message) {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(27,15);
  display.print(message);

  // update display with all of the above graphics
  display.display();
}
void UAS_driver::lcd_display_encoder_data() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(20,0);
  display.print(String("tick:") + String(encoder_cur_tick, DEC));
  display.setCursor(35, 0);
  display.print(String("speed:") + String(current_speed, DEC));
  // update display with all of the above graphics
  display.display();
}
