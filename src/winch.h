#ifndef WINCH_H
#define WINCH_H

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif


#include <globals.h>
#include <uas_servo.h>
#include <uas_motor.h>
#include <uas_comm.h>
#include <rc_module.h>
#include <uas_encoder.h>

namespace winch{
    extern uint16_t AUTO_RELEASE_DELTA_T;

    // TODO: there could be more different delta T for auto mode
    extern uint16_t MANUAL_DELTA_T;
    extern double K_P;
    extern double K_I;
    extern double K_D;


    enum Mode {RELEASE = 1, RETRACT, PRE_MISSION_IDLE , MISSION_IDLE, POST_MISSION_IDLE };

    enum ControllMode {AUTO = 1, MANUAL, RESET};
    
    enum ErrorFlags
    {
        RC_INPUT_ERROR = 1,
        ENCODER_READING_ERROR = 2,
        SPEED_ERROR = 4,
        OTHER_ERROR = 8
    };
    inline ErrorFlags operator|=( ErrorFlags a,  ErrorFlags b){
        return static_cast< ErrorFlags>(static_cast<int>(a) | static_cast<int>(b));
    }
    enum Status {NORMAL = 1, DEBUG, FAILSAFE, ERROR};

    class Winch{

    public:
        Winch();

        ~Winch();

        void winchSetUp();

        Mode current_mode;

        ControllMode current_controll_mode;

        Status current_status;

        ErrorFlags current_error;

        uint16_t status_led_delay;

        uint16_t steady_state_speed;

        /**
         * Function that updates RC input on interval
         * 
         */ 
        void updateRCInput();

        /**
         * For Auto mode trigger only. 
         * 
         */ 
        void updateRCTriggerInput();

        /**
         * 
         * 
         */ 
        void updateRCModeInput();

        void updateSystemStatusLED();

        /**
         * 
         * 
         */ 
        void updateDropSpeed();

        void updateComm();
        
        // =========== Mode controll functions ================
        void autoMode();

        /**
         * 
         * 
         */ 
        void manualMode();

        void resetMode();
        

        // =========== Mode controll functions ================

        // ===============Auto Mode controll functions ======
        void release();

        void retract(); 

        void missionIdle();

        void postMissionIdle();

        void preMissionIdle();

        /**
         * controller function that contain PID
         */ 
        void releaseController();
        // ===============Auto Mode controll functions ======


        void winchDebugMessage();



    private:
        rc::RCModule * rc;
        motor::UASMotor * motor;
        encoder::UASEncoder * encoder;
        comm::UASComm * xbee;
        uint16_t desired_drop_speed;
        uint16_t current_drop_speed;
        uint16_t released_rope_length;

        double last_error;
        double error_sum;

    };

}




#endif