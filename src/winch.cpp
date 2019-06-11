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

        start_of_release_position = 0;

        end_of_release_position = 0;

        current_PID_error = 0;
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
    }

    void Winch::updateSystemStatusLED(){
    }

    void Winch::updateComm(){
        xbee->commUpdateSignal();
    }

    void Winch::autoMode(){
        if (current_controll_mode == ControllMode::AUTO ){
            if (current_mode == Mode::PRE_MISSION_IDLE && rc->rc_winch_auto_mode_trigger.trigger){
                encoder->encoderReset();
                current_mode = Mode::RELEASE;

                xbee->winchDropTriggerStage();
                // delay(global::AUTO_RELEASE_TRIGGER_WAIT_TIME);
                start_of_release_position = encoder->encoderTotalDistance();

            }else if (current_mode == Mode::RELEASE){
                release();
            }else if (current_mode == Mode::RETRACT){
                retract();
            }else if(current_mode == Mode::MISSION_IDLE){
                missionIdle();
            }else if(current_mode == Mode::POST_MISSION_IDLE){
                postMissionIdle();
            }else{
                // there is a problem
            }
        }else{
            this->preMissionIdle();
        }
        // current_mode = Mode::RELEASE;
        // motor->motor_run_at(PIDcontroller(global::DESIRED_RELEASE_SPEED));     
        // current_mode = Mode::RETRACT;
        // retract();  
        
    }

    void Winch::manualMode(){
        if (current_controll_mode == ControllMode::MANUAL){
            motor->motor_run_at(rc->rc_winch_motor.percentage);
        }   
    }

    void Winch::resetMode(){
        if (current_controll_mode == ControllMode::RESET){
            // rc->rc_winch_motor.percentage = 0;
            motor->motor_run_at(0);
            encoder->encoderReset();
            rc->rc_winch_auto_mode_trigger.change = false;
            current_mode = Mode::PRE_MISSION_IDLE;
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

        if (current_mode == Mode::RELEASE && (current_speed != 0 || current_percentage > global::RELEASE_STALL_PERCENT) ){
            if (encoder->direction == encoder::Direction::DOWN){
                current_PID_error -= current_K_P * (desired_speed - current_speed) + current_K_D * (current_speed - last_speed);
            }else if (encoder->direction == encoder::Direction::UP){
                current_PID_error -= current_K_P * (desired_speed + current_speed) + current_K_D * (current_speed - last_speed);

            }
            // current_PID_error -= current_K_P * (desired_speed - current_speed) + current_K_D * (current_speed - last_speed);
            // current_PID_error += current_K_P * (desired_speed - current_speed);

        }else if (current_mode == Mode::RETRACT && ( current_speed != 0|| current_percentage > global::RETRACT_STALL_PERCENT) ){
            // current_PID_error += current_K_P * (desired_speed - current_speed);
            current_PID_error += current_K_P * (desired_speed - current_speed) + current_K_D * (current_speed - last_speed);
        }
        current_percentage = constrain(current_PID_error, 0, 100);
        
        if(current_percentage < 0){
            current_percentage = 0;
            // current_PID_error = 0;
        }else if (current_percentage > 100){
            current_percentage = 100;
            // current_PID_error = 100;
        }

        last_speed = current_speed;

        return int16_t(current_percentage);

    }

   void Winch::release(){
        auto_current_delta_t = global::AUTO_RELEASE_DELTA_T;
        // wait for the xbee. 
        if (xbee->current_signal == comm::Flags::ROVER_NEAR_GROUND){

            motor->motor_run_at(PIDcontroller(global::DESIRED_SLOW_RELEASE_SPEED));

        }else if (xbee->current_signal == comm::Flags::ROVER_RELEASED){

            end_of_release_position = encoder->encoderTotalDistance();

            current_mode = Mode::RETRACT;

        }else{
            motor->motor_run_at(PIDcontroller(global::DESIRED_RELEASE_SPEED));
        }  
        // motor->motor_run_at(PIDcontroller(global::DESIRED_RELEASE_SPEED)); 
    }

    void Winch::retract(){
        // TODO: FIX THIS!!!
        auto_current_delta_t = global::AUTO_RETRACT_DELTA_T;
        // if (encoder-> abs(encoder->encoderTotalDistance() - end_of_release_position) < (end_of_release_position -  start_of_release_position)){
        if (encoder->encoder->read() < 0){
            motor->motor_run_at(PIDcontroller(global::DESIRED_RETRACT_SPEED));
        }else{

            current_mode = Mode::POST_MISSION_IDLE;
        }

        // motor->motor_run_at(PIDcontroller(global::DESIRED_RETRACT_SPEED));
    }

    void Winch::missionIdle(){
        // after release,, this will hang on Rover. 
    }

    void Winch::postMissionIdle(){
        // Make sure that the rope is holden down
        auto_current_delta_t = global::AUTO_POST_MISSION_IDLE_DELTA_T;
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
        xbee->serialDebug();

        Serial.printf(F("=====Controller Debug ========\n"));
        Serial.printf(F("Retract: start: %d, end %d, curent: %d\n"), start_of_release_position, end_of_release_position,encoder->encoderTotalDistance() );
        Serial.printf(F("current pid error: %f\n"), current_PID_error);
        // Serial.printf(F("current error sum: %f\n"), error_sum);
        Serial.printf(F("current mode: %d (1 is release, 2 is retract)\n"), current_mode);
        Serial.printf(F("current controll mode (1 is auto): %d\n"), current_controll_mode);
        Serial.printf(F("current percentage: %f\n\n"), current_percentage);
    }
};

