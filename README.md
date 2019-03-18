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
|reset | | | 
|total_distance | | | 
|valid | | | 
|total_distance | | | 
|current_speed | | | 
|set_desired_speed | | | 
| | | | 

#### Motor

We are planning to use motor mainly for retracting the rope. It has two actions: start and stop. 

As a back up action to map the speed to motor control, we have Motor.run_at()

|name |return | parameter |description |
|---|---|---|---|
|set_range | | | | 
|set_retract | | | | 
|set_brake_range | | | | 
|stop | | | | 
|start | | | | 
|run_at | | | | 


#### Servo



|name |return | parameter |description |
|---|---|---|---|
|set_stop | | |
|set_brake_range | | | | 
|set_release | | | | 
|stop | | | | 
|brake | | | | 
|release | | | | 
| | | | | 


### Controller modules



## Set up
platformio init --ide clion --board uno
> This will start the project. uno is chosen here but feel free to change it later in [platofrmio.ini](./platformio.ini#13)

## Developement

I set things up on Clion, but feel free to use Vscode. Sometimes Vscode is better, if all the button works. 

### run the project
