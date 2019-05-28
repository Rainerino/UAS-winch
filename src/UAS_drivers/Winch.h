#ifndef WINCH_H
#define WINCH_H
 
#include "Globals.h"
#include "UAS_driver.h"

namespace winch{

    class Winch{
    public:

        Winch();

        enum Mode {PRE_MISSION_IDLE , RELEASE, RETRACT, MISSION_IDLE, POST_MISSION_IDLE };

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


    private:
        UAS_driver driver;
    };
}




#endif