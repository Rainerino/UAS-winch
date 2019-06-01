#include "winch.h"
#include <Ticker.h>

winch::Winch * uas_winch;

void communicationCallback(){
    uas_winch->updateComm();
}

void statusLEDCallback(){
    // uas_winch->updateSystemStatusLED();
    uas_winch->winchDebugMessage();
}

void rcUpdateCallback(){
    uas_winch->updateRCInput();
}

void autoModeCallback(){
    uas_winch->autoMode();
}
void manualModeCallback(){
    uas_winch->manualMode();
}

void encoderSpeedCallback(){
    uas_winch->updateDropSpeed();
}

Ticker rc_update(rcUpdateCallback, global::RC_DELTA_T, 0);
Ticker encoder_speed(encoderSpeedCallback, encoder::SPEED_DELTA_T, 0);

Ticker auto_mode_update(autoModeCallback, 100, 0);
Ticker manual_mode_update(manualModeCallback, 100, 0);

Ticker comm_update(communicationCallback, comm::COMM_DELTA_T, 0 );
Ticker status_update(statusLEDCallback,500, 0);


void setup(){
    delay(3000);
    Serial.println("start setup");
    uas_winch = new winch::Winch();
    Serial.println("start setup1");
    uas_winch->winchSetUp();
    Serial.println("start setup2");

    // comm_update.start();
    status_update.start();
    encoder_speed.start();
    rc_update.start();
    auto_mode_update.start();
    manual_mode_update.start();

}

void loop(){
    // comm_update.update();
    status_update.update();
    encoder_speed.update();
    rc_update.update();
    auto_mode_update.update();
    manual_mode_update.update();
}
