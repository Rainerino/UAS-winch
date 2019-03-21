# UAS-winch

## TODO

- change TIcker with TimerOne? https://www.pjrc.com/teensy/td_libs_TimerOne.html
- more testing files. Maybe use C++ testing frameworks, since pio testing is not free (well..., reasonable business module)
- figure out the direction of the motor
- consider power on motor for releasing?
- find a documentation tool, that is better than MARKDOWN LOL
- finish LCD display driver

## Description

This program is designed, implemented and tested for UAS's winch system. It's writen was platoformio on Clion and VSCode in c and c++.
The testing uses travis.

## Operation

> Note: after manual to auto switch, the mission will be reseted automatically

| mode | controls | description |comment |
|---|---|---|---|
|operation mode |mapped to rc_op_mode, let you switch between operation mode and auto mode  | |
|auto|failsafe brake| | |
| |reset | | |
| | | | |
|manual |failsafe | | |
|       |speed control |mapped with rc_speed_ctrl | |
|       |control the retraction mode |mapped with rc_ctrl_mode| |
        

## Interface set ups

|name| type of signal (input/output)|port number|Set up |Comments|
|---|---|---|---|---|
|motor|output |enA on 5, in1, in2 on 4, 6 | ??????| in1, in2 control the direction| 
|servo|output|9, 5V, GND| |Make sure that it's powered with a true 5V instead of just the microcontroller|
|encoder|input|2, 3, 5V, GND| | read the ticks from encoder |
|rc_failsafe|input|A2|HIGH: failsafe triggered, LOW: failsafe disengaged |instant brake, overwrite all controls. Used as failsafe|
|rc_speed_ctrl|input|A1|Map a standard PWM 1000-2000 input linearly |speed controller (throttle). Brake for release, motor for retract.|
|rc_op_mode|input|A3|HIGH: AUTO mode, LOW: Manual Mode |trigger manual input or auto|
|rc_ctrl_mode|input|A4| LOW < IDLE, RETRACT, RELEASE < HIGH| mode: release, retract or idle|

### Driver modules

#### Encoder (NOT UPDATED)
 
Encoder module that were used as the input feedback  

|name |return | parameter |description |
|---|---|---|---|
|encoder_reset |void |void | | hard reset all the encoder parameters 
|encoder_total_distance | unsigned long (mm)| void |Long variables are extended size variables for number storage, and store 32 bits (4 bytes), from -2,147,483,648 to 2,147,483,647. Since we are not concerned about having 2,000,000 meter ropes so long would do nicely. 
|encoder_valid |bool | void| check if we have exceed the maximum rpm 
|encoder_current_speed |void |unsigned int | termianl velocity is 55m/s, we are fine with having capped at 66 m/s 
|encoder_set_max_rpm |int |void | 

> Reason why not to use int: **On the Arduino Uno (and other ATmega based boards) an int stores a 16-bit (2-byte) value. This yields a range of -32,768 to 32,767**

See encoder at: https://www.pjrc.com/teensy/td_libs_Encoder.html

#### Motor (NOT UPDATED)

We are planning to use motor mainly for retracting the rope. It has two actions: start and stop. Although a linear controller is included, based on the max toque of the motor, having it to fully control the winch is difficult. 

As a back up action to map the speed to motor control, we have Motor.run_at()

|name |return | parameter |description |
|---|---|---|---|
|motor_set_range |void | int low, high, steps| | 
|~~motor_set_brake_range~~ | void | int low, high, steps| | 
|motor_stop | void|void | | 
|motor_start |void |void | | 
|motor_run_at |void |float  |precent from 1 to 0 | 


#### Servo (NOT UPDATED)


|name |return | parameter |description |
|---|---|---|---|
|servo_set_stop |void | int degree|
|servo_set_brake_range | void| | | 
|servo_set_release |void |int degree | | 
|servo_stop |void |int degree | | 
|servo_brake_at |void |float percent | | 
|servo_release | void| int degree| | 
| | | | | 


### Controller modules

|name |return | parameter |description |
|---|---|---|---|
|release |bool | void|if true, the release is completed | 
|retract | bool| void| |
|change_speed|void|int speed |takes the current speed, and output a new speed. Since this is a nonlinear control, we are just going to keep    | 
| | | | |

For rc: https://www.sparkfun.com/tutorials/348
with the possiblity of using using ppm, since there are lots of channels: https://www.instructables.com/id/Reading-RC-Receiver-PPM-Signal-Using-Arduino/


### Display

[OLED module](https://www.banggood.com/0_91-Inch-128x32-IIC-I2C-Blue-OLED-LCD-Display-DIY-Oled-Module-SSD1306-Driver-IC-DC-3_3V-5V-p-1140506.html?rmmds=search&cur_warehouse=USA) from Banggoood 

#### Set up
Follow this [guide](https://startingelectronics.org/tutorials/arduino/modules/OLED-128x64-I2C-display/). **Note that the guide is based on 128x64, the one we have is 128x32**



## Set up and run project
```bash
git clone https://github.com/Rainerino/UAS-winch.git
```

if you dont have platformio installed: 
```bash
pip install -U platformio
```
To enable CLI mode, add this to /.bashrc or /.profile:
```bash
export PATH=$PATH:~/.platformio/penv/bin
```

Test run: 

```bash
pio -f -c clion run --target upload
pio -f -c clion run  # just run
```

## Development

I set things up on Clion, but feel free to use Vscode. Sometimes Vscode is better, if all the button works. 




