#include "winch.h"
#include <globals.h>
#include <Ticker.h>

winch::Winch uas_winch;

void communicationCallback(){
    uas_winch.commUpdate();
}

void statusLEDCallback(){
    uas_winch.updateSystemStatusLED();
}
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

Ticker rc_update(rcUpdateCallback, global::RC_DELTA_T, 0);
Ticker encoder_speed(encoderSpeedCallback, global::SPEED_DELTA_T, 0);
Ticker auto_mode_update(autoModeCallback, 100, 0);
Ticker manual_mode_update(manualModeCallback, 100, 0);
Ticker winch_speed_update(encoderSpeedCallback, 100, 0);
Ticker comm_update(communicationCallback, global::COMM_DELTA_T, 0 );
Ticker status_update(statusLEDCallback, global::STATUS_DELTA_T, 0);


void setup(){
    uas_winch = winch::Winch();
    uas_winch.winchSetUp();

    comm_update.start();
    status_update.start();
    encoder_speed.start();
    rc_update.start();
    auto_mode_update.start();
    manual_mode_update.start();
}

void loop(){
    comm_update.update();
    status_update.update();
    encoder_speed.update();
    rc_update.update();
    auto_mode_update.update();
    manual_mode_update.update();
}
