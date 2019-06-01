#include "winch.h"

namespace winch{


    Winch::Winch()
        : rc(new rc::RCModule()) ,
          motor(new motor::UASMotor(IN1_PIN, IN2_PIN)),
          encoder( new encoder::UASEncoder(A_SIGNAL, B_SIGNAL)),
          xbee(new comm::UASComm(comm::WINCH_SYSTEM))
    {

        motor->motor_run_at(0);

        current_mode = Mode::PRE_MISSION_IDLE;

        current_percentage = global::INITIAL_PERCENT;
    }

    Winch::~Winch(){
        delete rc;
        delete motor;
        delete encoder;
        delete xbee;
    }

    void Winch::winchSetUp(){
        rc->attachRCPin(rc->rc_winch_auto_mode_trigger, RC_WINCH_TRIGGER_PIN);
        rc->attachRCPin(rc->rc_winch_mode, RC_WINCH_MODE_PIN);
        rc->attachRCPin(rc->rc_winch_motor, RC_WINCH_MOTOR_PIN);

    }
    void Winch::updateRCInput(){
        this->updateRCModeInput();
        this->updateRCTriggerInput();
    }

    void Winch::updateRCTriggerInput(){
        if(!rc->rcWinchAutoModeTriggerReading()){
            current_status = ERROR;
            current_error |= RC_INPUT_ERROR;
        }else{
            xbee->triggerDrop();
        }
    }
    void Winch::updateRCModeInput(){
        if(!rc->rcWinchMotorReading()){
            current_status = ERROR;
            current_error |= RC_INPUT_ERROR;
        }

        // mode reading and switching. 
        if (!rc->rcWinchModeReading()){
            current_status = ERROR;
            current_error |= RC_INPUT_ERROR;
        }else{//TODO SWITCH
            if (rc->rc_winch_mode.mode == 0){
                // the deafult configuration is reset
                current_controll_mode = RESET;
            }else if(rc->rc_winch_mode.mode == 1){
                current_controll_mode = AUTO;
            }else if(rc->rc_winch_mode.mode == 2){
                current_controll_mode = MANUAL;
            }else{
            }
        }
    }

    void Winch::updateDropSpeed(){
        current_speed =  encoder->encoderUpdateCurrentSpeed(encoder::SPEED_DELTA_T);
        released_rope_length =  encoder->encoderTotalDistance();
    }

    void Winch::updateSystemStatusLED(){
    }

    void Winch::updateComm(){
        xbee->commUpdateSignal();
    }

    void Winch::autoMode(){
        if (current_controll_mode == AUTO){
            current_mode = Mode::RELEASE;
            motor->motor_run_at(PIDcontroller());
        }

        // if (current_controll_mode == AUTO && rc->rc_winch_auto_mode_trigger.trigger){
        //     // TODO: add reset and check change
        //     if (rc->rc_winch_auto_mode_trigger.change){
        //         current_mode = RELEASE;
        //         rc->rc_winch_auto_mode_trigger.change = false;
        //     }
        //     if (current_mode == RELEASE){
        //         this->release();
        //     }else if (current_mode == RETRACT){
        //         this->retract();
        //     }else if(current_mode == MISSION_IDLE){
        //         this->missionIdle();
        //     }else if(current_mode == POST_MISSION_IDLE){
        //         this->postMissionIdle();
        //     }else{
        //     }
        // }else{
        //     this->preMissionIdle();
        // }

    }

    void Winch::manualMode(){
        if (current_controll_mode == MANUAL){
            motor->motor_run_at(rc->rc_winch_motor.percentage);
        }
    }

    void Winch::resetMode(){
        if (current_controll_mode == RESET){
            // rc->rc_winch_motor.percentage = 0;
            current_PID_error = 0;
            error_sum = 0;
            current_percentage = global::INITIAL_PERCENT;
            motor->motor_run_at(0);
        }

    }

    int16_t Winch::PIDcontroller(){
        if(current_mode == Mode::RELEASE){
            current_K_P = global::K_P_RELEASE;
            current_K_I = global::K_I_RELEASE;
            current_K_D = global::K_D_RELEASE;

            desired_speed = global::DESIRED_RELEASE_SPEED;

        }else if(current_mode == Mode::RETRACT){
            current_K_P = global::K_P_RETRACT;
            current_K_I = global::K_I_RETRACT;
            current_K_D = global::K_D_RETRACT;

            desired_speed = global::DESIRED_RETRACT_SPEED;
        }
        double delta_speed = current_speed - last_speed;
        current_PID_error = current_K_P * current_PID_error + current_K_D * delta_speed;

        if(encoder->direction == encoder::Direction::UP){
            if (current_mode == Mode::RELEASE){
                current_PID_error = desired_speed + current_speed;
                current_percentage = current_percentage - current_PID_error / desired_speed;
            }else if (current_mode == Mode::RETRACT){
                current_PID_error = desired_speed - current_speed;
                current_percentage = current_percentage + current_PID_error / desired_speed;
            }
        }else{
           if (current_mode == Mode::RELEASE){
                current_PID_error = desired_speed - current_speed;
                current_percentage = current_percentage + current_PID_error / desired_speed;
            }else if (current_mode == Mode::RETRACT){
                current_PID_error = desired_speed + current_speed;
                current_percentage = current_percentage + current_PID_error / desired_speed;
            }
        }
        current_percentage = constrain(current_percentage, 0, 100);
        
        if(current_percentage < 0){
            current_percentage = 0;
        }else if (current_percentage > 100){
            current_percentage = 100;
        }
        last_speed = current_speed;

        return int16_t(current_percentage);

    }
    // int16_t Winch::releasePIDController(){
    //     // current_release_error = desired_release_speed - current_release_speed;
    //     // double delta_release_speed = current_release_speed - last_release_speed;

    //     // current_release_error = global::K_P_RELEASE * current_release_error + global::K_D_RELEASE * delta_release_speed ;
        
    
    //     if (encoder->direction == encoder::Direction::UP){

    //         current_release_error = desired_release_speed + current_release_speed;

    //         double delta_release_speed = current_release_speed - last_release_speed;

    //         current_release_error = global::K_P_RELEASE * current_release_error + global::K_D_RELEASE * delta_release_speed;

    //         current_release_percentage = current_release_percentage - current_release_error / desired_release_speed;

    //     }else{
    //         current_release_error = desired_release_speed - current_release_speed;

    //         double delta_release_speed = current_release_speed - last_release_speed;

    //         current_release_error = -1 * global::K_P_RELEASE * current_release_error + global::K_D_RELEASE * delta_release_speed;

    //         current_release_percentage = current_release_percentage + current_release_error / desired_release_speed;
    //     }

    //     current_release_percentage = constrain(current_release_percentage, 0, 100);
        
    //     if(current_release_percentage < 0){
    //         current_release_percentage = 0;
    //     }else if (current_release_percentage > 100){
    //         current_release_percentage = 100;
    //     }

    //     last_release_speed = current_release_speed;

    //     return int16_t(current_release_percentage);
    // }


    // int16_t Winch::retractPIDController(){

    //     if (encoder->direction == encoder::Direction::UP){
    //         // desired: up 1000mm/s
    //         current_retract_error =  desired_retract_speed - current_retract_speed;

    //         double delta_speed = current_retract_speed - last_retract_speed;
    //     // error_sum += global::K_I * current_drop_speed;
    //     // error_sum = constrain(error_sum, global::I_MIN, global::I_MAX);
    //     // current_PID_error = global::K_P * current_PID_error + global::K_I * error_sum  + global::K_D * delta_error;
    //         current_retract_error = global::K_P * current_retract_error + global::K_D * delta_speed;

    //         current_retract_percentage  = current_retract_percentage +  current_retract_error / desired_retract_speed;
    //     }else{
    //         current_retract_error =  desired_retract_speed + current_retract_speed;

    //         double delta_speed = current_retract_speed - last_retract_speed;
    //     // error_sum += global::K_I * current_drop_speed;
    //     // error_sum = constrain(error_sum, global::I_MIN, global::I_MAX);
    //     // current_PID_error = global::K_P * current_PID_error + global::K_I * error_sum  + global::K_D * delta_error;
    //         current_retract_error = global::K_P * current_retract_error + global::K_D * delta_speed;

    //         current_retract_percentage  = current_retract_percentage +  current_retract_error / desired_retract_speed;
    //     }
   
    //     return int16_t(current_percentage);
    // }





    void Winch::missionIdle(){

    }

    void Winch::postMissionIdle(){

    }

    void Winch::preMissionIdle(){

    }

    void Winch::winchDebugMessage(){
        rc->rcSerialDebug();
        encoder->encoderSerialDebug();
        motor->motorSerialDebug();

        Serial.printf(F("=====Controller Debug ========\n"));
        Serial.printf(F("current pid error: %f\n"), current_PID_error);
        Serial.printf(F("current error sum: %f\n"), error_sum);
        Serial.printf(F("current mode: %d\n"), current_mode);
        Serial.printf(F("current controll mode: %d\n"), current_controll_mode);
        Serial.printf(F("current percentage: %f\n\n"), current_percentage);
    }
};

