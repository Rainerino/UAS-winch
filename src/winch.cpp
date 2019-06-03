#include "winch.h"

namespace winch{


    Winch::Winch()
        : rc(new rc::RCModule()) ,
          motor(new motor::UASMotor(IN1_PIN, IN2_PIN, ENA_PIN)),
          encoder( new encoder::UASEncoder(A_SIGNAL, B_SIGNAL)),
          xbee(new comm::UASComm(comm::System::WINCH_SYSTEM))
    {

        motor->motor_run_at(0);

        current_mode = Mode::PRE_MISSION_IDLE;

        current_percentage = global::INITIAL_PERCENT;

        current_status = NORMAL;

        current_error = ErrorFlags::NONE;

        current_controll_mode = ControllMode::RESET;

        auto_current_delta_t = global::AUTO_RELEASE_DELTA_T;

        starting_position = 0;

        end_of_release_position = 0;
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
            current_error |= ErrorFlags::RC_INPUT_ERROR;
        }else{
            xbee->triggerDrop();
        }
    }
    void Winch::updateRCModeInput(){
        if(!rc->rcWinchMotorReading()){
            current_status = ERROR;
            current_error |= ErrorFlags::RC_INPUT_ERROR;
        }

        // mode reading and switching. 
        if (!rc->rcWinchModeReading()){
            current_status = ERROR;
            current_error |= ErrorFlags::RC_INPUT_ERROR;
        }else{//TODO SWITCH
            if (rc->rc_winch_mode.mode == 0){
                // the deafult configuration is reset
                current_controll_mode = ControllMode::RESET;
            }else if(rc->rc_winch_mode.mode == 1){
                current_controll_mode = ControllMode::AUTO;
            }else if(rc->rc_winch_mode.mode == 2){
                current_controll_mode = ControllMode::MANUAL;
            }else{
            }
        }
    }

    void Winch::updateDropSpeed(){
        current_speed =  encoder->encoderUpdateCurrentSpeed(global::SPEED_DELTA_T);
        released_rope_length =  encoder->encoderTotalDistance();
    }

    void Winch::updateSystemStatusLED(){
    }

    void Winch::updateComm(){
        xbee->commUpdateSignal();
    }

    void Winch::autoMode(){
        if (current_controll_mode == ControllMode::AUTO && rc->rc_winch_auto_mode_trigger.trigger){
            
            #if UAS_DEBUG
                Serial.println("============DEBUG MODE!");
            #else
                Serial.println("WHAEVER?");
            #endif

            if (rc->rc_winch_auto_mode_trigger.change){
                encoder->encoderReset();
                current_mode = Mode::RELEASE;
                xbee->current_signal += comm::Flags::WINCH_DROP_TRIGGERED;
                rc->rc_winch_auto_mode_trigger.change = false;
                delay(global::AUTO_RELEASE_TRIGGER_WAIT_TIME);
                starting_position = encoder->encoderTotalDistance();
            }
            if (current_mode == Mode::RELEASE){
                this->release();
            }else if (current_mode == Mode::RETRACT){
                this->retract();
            }else if(current_mode == Mode::MISSION_IDLE){
                this->missionIdle();
            }else if(current_mode == Mode::POST_MISSION_IDLE){
                this->postMissionIdle();
            }else{
                // there is a problem
            }
        }else{
            this->preMissionIdle();
        }

    }

    void Winch::manualMode(){
        if (current_controll_mode == ControllMode::MANUAL){
            motor->motor_run_at(rc->rc_winch_motor.percentage);
        }
    }

    void Winch::resetMode(){
        if (current_controll_mode == ControllMode::RESET){
            // rc->rc_winch_motor.percentage = 0;
            current_PID_error = 0;
            error_sum = 0;
            motor->motor_run_at(0);
            encoder->encoderReset();
        }
    }

    int16_t Winch::PIDcontroller(int16_t input_speed){
        desired_speed = input_speed;

        if(current_mode == Mode::RELEASE){
            current_K_P = global::K_P_RELEASE;
            current_K_I = global::K_I_RELEASE;
            current_K_D = global::K_D_RELEASE;
        }else if(current_mode == Mode::RETRACT){
            current_K_P = global::K_P_RETRACT;
            current_K_I = global::K_I_RETRACT;
            current_K_D = global::K_D_RETRACT;
        }

        double delta_speed = current_speed - last_speed;
        current_PID_error = current_K_P * current_PID_error + current_K_D * delta_speed;

        if(encoder->direction == encoder::Direction::UP){
            if (current_mode == Mode::RELEASE){
                current_PID_error = desired_speed + current_speed;
                // this is where the counter stoped the contorl and set it to 0. 
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

   void Winch::release(){
        // wait for the xbee. 
        if (xbee->current_signal & comm::Flags::ROVER_NEAR_GROUND && !(xbee->current_signal & comm::Flags::ROVER_RELEASED)){
            motor->motor_run_at(PIDcontroller(global::DESIRED_SLOW_RELEASE_SPEED));
        }else if (xbee->current_signal & comm::Flags::ROVER_RELEASED){
            end_of_release_position = encoder->encoderTotalDistance();
            current_mode = Mode::RETRACT;
            released_rope_length = end_of_release_position - starting_position;
        }else{
            motor->motor_run_at(PIDcontroller(global::DESIRED_RELEASE_SPEED));
        }   
    }

    void Winch::retract(){
        if (encoder->encoderTotalDistance() > starting_position){
            motor->motor_run_at(PIDcontroller(global::DESIRED_RETRACT_SPEED));
        }else{
            current_mode = Mode::POST_MISSION_IDLE;
        }
    }

    void Winch::missionIdle(){
        // after release,, this will hang on Rover. 
    }

    void Winch::postMissionIdle(){
        // Make sure that the rope is holden down
        // TODO: tune this number
        motor->motor_run_at(global::POST_MISSION_IDLE_PERCENT);
    }

    void Winch::preMissionIdle(){
        // not sure if there are anything for this
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

