#include "winch.h"

namespace winch{
    Winch::Winch(){
    }

    Winch::~Winch(){
        delete rc;
        delete motor;
    }

    void Winch::winchSetUp(){
        Serial.begin(9600);
        rc = new rc::RCModule();
        rc->attachRCPin(& rc->rc_winch_auto_mode_trigger, RC_WINCH_TRIGGER_PIN);
        rc->attachRCPin(& rc->rc_winch_mode, RC_WINCH_MODE_PIN);
        rc->attachRCPin(& rc->rc_winch_motor, RC_WINCH_MOTOR_PIN);

        motor = new motor::UASMotor(in1, in2);

    }
    void Winch::updateRCInput(){
        this->updateRCModeInput();
        this->updateRCTriggerInput();
    }

    void Winch::updateRCTriggerInput(){
        if(!rc->rcWinchAutoModeTriggerReading()){
            this->current_status = this->ERROR;
            this->current_error |= this->RC_INPUT_ERROR;
        }
    }
    void Winch::updateRCModeInput(){
        if(!rc->rcWinchMotorReading()){
            this->current_status = this->ERROR;
            this->current_error |= this->RC_INPUT_ERROR;
        }

        if (!rc->rcWinchModeReading()){
            this->current_status = this->ERROR;
            this->current_error |= this->RC_INPUT_ERROR;
        }else{
            if (rc->rc_winch_mode.mode == 0){
                this->current_controll_mode = this->AUTO;
            }else if(rc->rc_winch_mode.mode == 1){
                this->current_controll_mode = this->MANUAL;
            }else if(rc->rc_winch_mode.mode == 2){
                this->current_controll_mode = this->RESET;
            }else{
            }
        }

    }
    void Winch::updateSystemStatusLED(){

    }

    void Winch::autoMode(){
        if (this->current_controll_mode == this->AUTO){
            // TODO add reset and check change
            if (this->current_mode == this->PRE_MISSION_IDLE){
                this->preMissionIdle();
            }else if (this->current_mode == this->RELEASE){
                this->release();
            }else if (this->current_mode == this->RETRACT){
                this->retract();
            }else if(this->current_mode == this->MISSION_IDLE){
                this->missionIdle();
            }else if(this->current_mode == this->POST_MISSION_IDLE){
                this->postMissionIdle();
            }else{

            }
        }

    }
    void Winch::manualMode(){
        if (this->current_controll_mode == this->MANUAL){
            if (rc->rc_winch_motor.percentage > 0){
                motor->motor_run_at(rc->rc_winch_motor.percentage);
            }else{
                motor->motor_run_at(rc->rc_winch_motor.percentage * -1);
            }
        }
    }

    void Winch::release(){

    }

    void Winch::retract(){
        
    }

    void Winch::releaseController(){

    }

    void Winch::missionIdle(){

    }

    void Winch::postMissionIdle(){

    }

    void Winch::preMissionIdle(){

    }

    void Winch::dropSpeedUpdate(){
        // driver.encoder_update_current_speed(global::SPEED_DELTA_T, uas_encoder);
    }

    void Winch::commUpdate(){

    }
};

