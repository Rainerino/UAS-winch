#include "winch.h"

namespace winch{
    uint16_t AUTO_RELEASE_DELTA_T = 100;

    uint16_t MANUAL_DELTA_T = 100;

    double K_P = 1.0;
    double K_I = 1.0;
    double K_D = 1.0;

    Winch::Winch(){
    }

    Winch::~Winch(){
        delete rc;
        delete motor;
        delete encoder;
        delete xbee;
    }

    void Winch::winchSetUp(){
        Serial.begin(9600);
        rc = new rc::RCModule();
        rc->attachRCPin(& rc->rc_winch_auto_mode_trigger, RC_WINCH_TRIGGER_PIN);
        rc->attachRCPin(& rc->rc_winch_mode, RC_WINCH_MODE_PIN);
        rc->attachRCPin(& rc->rc_winch_motor, RC_WINCH_MOTOR_PIN);

        motor = new motor::UASMotor(in1, in2);

        encoder = new encoder::UASEncoder(A_SIGNAL, B_SIGNAL);
        
        xbee = new comm::UASComm(comm::WINCH_SYSTEM);

    }
    void Winch::updateRCInput(){
        this->updateRCModeInput();
        this->updateRCTriggerInput();
    }

    void Winch::updateRCTriggerInput(){
        if(!rc->rcWinchAutoModeTriggerReading()){
            this->current_status = ERROR;
            this->current_error |= RC_INPUT_ERROR;
        }else{
            xbee->triggerDrop();
        }
    }
    void Winch::updateRCModeInput(){
        if(!rc->rcWinchMotorReading()){
            this->current_status = ERROR;
            this->current_error |= RC_INPUT_ERROR;
        }

        // mode reading and switching. 
        if (!rc->rcWinchModeReading()){
            this->current_status = ERROR;
            this->current_error |= RC_INPUT_ERROR;
        }else{
            if (rc->rc_winch_mode.mode == 0){
                // the deafult configuration is reset
                this->current_controll_mode = RESET;
            }else if(rc->rc_winch_mode.mode == 1){
                this->current_controll_mode = AUTO;
            }else if(rc->rc_winch_mode.mode == 2){
                this->current_controll_mode = MANUAL;
            }else{
            }
        }
    }

    void Winch::updateDropSpeed(){
        this->current_drop_speed =  encoder->encoderUpdateCurrentSpeed(encoder::SPEED_DELTA_T);
        this->released_rope_length =  encoder->encoderTotalDistance();
    }

    void Winch::updateSystemStatusLED(){
        
    }

    void Winch::updateComm(){
        xbee->commUpdateSignal();
    }

    void Winch::autoMode(){
        if (this->current_controll_mode == AUTO && rc->rc_winch_auto_mode_trigger.trigger){
            // TODO: add reset and check change
            if (this->current_mode == PRE_MISSION_IDLE){
                this->preMissionIdle();
            }else if (this->current_mode == RELEASE){
                this->release();
            }else if (this->current_mode == RETRACT){
                this->retract();
            }else if(this->current_mode == MISSION_IDLE){
                this->missionIdle();
            }else if(this->current_mode == POST_MISSION_IDLE){
                this->postMissionIdle();
            }else{
            }
        }

    }
    void Winch::manualMode(){
        if (this->current_controll_mode == MANUAL){
            if (rc->rc_winch_motor.percentage > 0){
                motor->motor_run_at(rc->rc_winch_motor.percentage);
            }else{
                motor->motor_run_at(rc->rc_winch_motor.percentage * -1);
            }
        }
    }

    void Winch::resetMode(){

    }
    void Winch::release(){
        if (xbee->current_signal == comm::IDLE){
            // release the Rover
            xbee->current_signal += comm::WINCH_DROP_TRIGGERED;
        }else{
            // if not getting signal from Rover to stop:AUTO
            if (xbee->current_signal | comm::ROVER_RELEASED){
                // TODO: add some delay
                this->current_mode = winch::RETRACT;
            }else{
                // controller loop. 
                this->desired_drop_speed = (xbee->current_signal | comm::ROVER_NEAR_GROUND) 
                ? global::DESIRED_SLOW_RELEASE_SPEED : global::DESIRED_RELEASE_SPEED;
                this->releaseController();
            }
        }



    }
    void Winch::releaseController(){
        // TODO: Finish the controller
        double error = this->desired_drop_speed - this->current_drop_speed;
        double delta_error = error - this->last_error;
        this->error_sum += error;
        error = K_P * error + K_I * error_sum  + K_D * delta_error;
        this->last_error = error;  

    }

    void Winch::retract(){

    }



    void Winch::missionIdle(){

    }

    void Winch::postMissionIdle(){

    }

    void Winch::preMissionIdle(){

    }

    void Winch::winchDebugMessage(){
        rc->rcSerialDebug();
        encoder->encoderSerialDebug();
    }
};

