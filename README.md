# UAS-winch

## Description

This program is designed, implemented and tested for UAS's winch system. It's writen was platoformio on Clion and VSCode in c and c++.
The testing uses travis.


|name| type of signal (input/output)|port number|description|
|---|---|---|---|
|motor|output |enA | |
|servo|output|9| |
|encoder|input||read the ticks from encoder |
|rc_in_1|input|12|trigger signal of the release|
|rc_in_2|input||instant brake, overwrite all controls. Used as failsafe|
|rc_in_3|input| |360 degree diaos on the transmitter. used to control the speed.|
|rc_in_4|input| |trigger the retraction of the rope |
|rc_in_5|input| |stop the motor.|
|rc_in_6|input| |stop the rope from going down. A soft brake.|

### Driver modules

#### Encoder
 
 
 
|name |return | parameter |description |
|---|---|---|---|
|encoder_reset |void |void | | hard reset all the encoder parameters 
|encoder_total_distance | unsigned long (mm)| void |Long variables are extended size variables for number storage, and store 32 bits (4 bytes), from -2,147,483,648 to 2,147,483,647. Since we are not concerned about having 2,000,000 meter ropes so long would do nicely. 
|encoder_valid |bool | void| check if we have exceed the maximum rpm 
|encoder_current_speed |void |unsigned int | termianl velocity is 55m/s, we are fine with having capped at 66 m/s 
|encoder_set_max_rpm |int |void | 

> Reason why not to use int: **On the Arduino Uno (and other ATmega based boards) an int stores a 16-bit (2-byte) value. This yields a range of -32,768 to 32,767**


#### Motor

We are planning to use motor mainly for retracting the rope. It has two actions: start and stop. 

As a back up action to map the speed to motor control, we have Motor.run_at()

|name |return | parameter |description |
|---|---|---|---|
|motor_set_range |void | int low, high, steps| | 
|~~motor_set_brake_range~~ | void | int low, high, steps| | 
|motor_stop | void|void | | 
|motor_start |void |void | | 
|motor_run_at |void |float  |precent from 1 to 0 | 


#### Servo



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
|start_release | | | |
|start_retracting | | | |
|stable_release_speed | | | |
| | | | |
| | | | |




## Set up
platformio init --ide clion --board uno
> This will start the project. uno is chosen here but feel free to change it later in [platofrmio.ini](./platformio.ini#13)

## Developement

I set things up on Clion, but feel free to use Vscode. Sometimes Vscode is better, if all the button works. 

### run the project
