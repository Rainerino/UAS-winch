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

    // TODO: use marcro for output
    enum class Mode {RELEASE = 1, RETRACT, PRE_MISSION_IDLE , MISSION_IDLE, POST_MISSION_IDLE };

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

        ~Winch(); // rule of 3 5 0
        // 3: 
        void winchSetUp();

        Mode current_mode;

        ControllMode current_controll_mode;

        Status current_status;

        ErrorFlags current_error;

        uint16_t status_led_delay;

        uint16_t steady_state_speed;

        // T t(1, 2);
        //void f(T&);
        // f(T(1,2))
        // f(t);
        // void f(T&& s)
        // template <typename T>
        // T&&&&&&&&&&&&& t;
        // void f(T&& t) {
        // C c(int i) : t(i) {
        //     t = T(i);
        // }
        // }
        //     t = std::move(s)
        // }

        /**
         * virtual void f();
         */

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
        void missionIdle();

        void postMissionIdle();

        void preMissionIdle();

        /**
         * controller function that contain PID
         */ 
        int16_t PIDcontroller();
        // int16_t retractPIDController();

        // int16_t releasePIDController();
        // ===============Auto Mode controll functions ======


        void winchDebugMessage();



    private:
        rc::RCModule * rc;
        motor::UASMotor * motor;
        encoder::UASEncoder * encoder;
        comm::UASComm * xbee; // TODO smart pointer


        // int16_t desired_release_speed;
        // int16_t current_release_speed;
        // double current_release_error;
        // int16_t last_release_speed;
        // double current_release_percentage; 


        // int16_t desired_retract_speed;
        // int16_t current_retract_speed;
        // int16_t last_retract_speed;
        // double current_retract_error;
        // double current_retract_percentage;
        

        int16_t desired_speed;
        int16_t current_speed;
        int16_t last_speed;
        double current_PID_error;
        double current_percentage;


        double current_K_P;
        double current_K_I;
        double current_K_D;

        uint16_t released_rope_length;

        double error_sum;


    };

}




#endif