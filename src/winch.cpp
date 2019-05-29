#include "winch.h"

namespace winch{
    Winch::Winch(){
    }

    void Winch::updateRCInput(){
        this->updateRCModeInput();
        this->updateRCTriggerInput();
    }

    void Winch::updateRCTriggerInput(){

    }
    void Winch::updateRCModeInput(){
        
    }

    void Winch::autoMode(){
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
            // ???
        }
    }
    void Winch::manualMode(){
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


};

