#include "UAS_drivers/Winch.h"
#include "Globals.h"
#include <Ticker.h>

using namespace winch;
Winch uas_winch;

void rcUpdateCallback(){
    uas_winch.updateRCInput();
    
}

void autoModeCallback(){
    uas_winch.autoMode();
}
void manualModeCallback(){
    uas_winch.manualMode();
}

void encoderSpeedCallback(){
    uas_winch.dropSpeedUpdate();
}

Ticker rc_update(rcUpdateCallback, RC_DELTA_T, 0);
Ticker encoder_speed(encoderSpeedCallback, SPEED_DELTA_T, 0);
Ticker auto_mode_update(autoModeCallback, 100, 0);
Ticker manual_mode_update(manualModeCallback, 100, 0);
Ticker winch_speed_update(encoderSpeedCallback, 100, 0);


void setup(){
    uas_winch = Winch();
    encoder_speed.start();
    rc_update.start();
    auto_mode_update.start();
    manual_mode_update.start();
}

void loop(){
    encoder_speed.update();
    rc_update.update();
    auto_mode_update.update();
    manual_mode_update.update();
}
