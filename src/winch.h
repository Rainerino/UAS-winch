#ifndef WINCH_H
#define WINCH_H

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "globals.h"

#include <uas_servo.h>
#include <uas_motor.h>
#include <uas_comm.h>
#include <rc_module.h>



namespace winch{

    class Winch{
    public:
        Winch();

        enum Mode {RELEASE, RETRACT, PRE_MISSION_IDLE , MISSION_IDLE, POST_MISSION_IDLE };

        enum Status {NORMAL, DEBUG, FAILSAFE};

        Mode current_mode;

        uint32_t current_speed; // in cm/s, always positive since it's speed

        int64_t released_rope_length; // in cm

        void updateRCInput();

        void updateRCTriggerInput();

        void updateRCModeInput();

        void updateSpeed();

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


    private:
        // UAS_driver driver;
        // Encoder uas_encoder;
    };
}




#endif