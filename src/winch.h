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

namespace winch{

    class Winch{
    public:
        Winch();

        ~Winch();

        void winchSetUp();

        enum Mode {RELEASE = 1, RETRACT, PRE_MISSION_IDLE , MISSION_IDLE, POST_MISSION_IDLE };

        enum ControllMode {AUTO = 1, MANUAL, RESET};
        enum ErrorFlags
        {
            RC_INPUT_ERROR = 1,
            CanFly = 2,
            EatsFish = 4,
            Endangered = 8
        };

        enum Status {NORMAL = 1, DEBUG, FAILSAFE, ERROR};

        Mode current_mode;

        ControllMode current_controll_mode;

        Status current_status;

        ErrorFlags current_error;

        uint32_t current_speed; // in cm/s, always positive since it's speed

        int64_t released_rope_length; // in cm

        void updateRCInput();

        void updateRCTriggerInput();

        void updateRCModeInput();

        void updateSpeed();

        void updateSystemStatusLED();

        void autoMode();

        void manualMode();

        void release();

        void retract(); 

        // read the serial port 

        void releaseController();

        void winchDebugMessage();

        void missionIdle();

        void postMissionIdle();

        void preMissionIdle();
        
        void dropSpeedUpdate();

        void commUpdate();
        


    private:
        rc::RCModule * rc;
        motor::UASMotor * motor;
        // UAS_driver driver;
        // Encoder uas_encoder;
    };
    inline Winch::ErrorFlags operator|=( Winch::ErrorFlags a,  Winch::ErrorFlags b){
        return static_cast< Winch::ErrorFlags>(static_cast<int>(a) | static_cast<int>(b));
    }
}




#endif