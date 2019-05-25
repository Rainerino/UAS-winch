#include "UAS_drivers/Winch.h"
#include "Globals.h"
#include <Ticker.h>

using namespace winch;
Winch uas_winch;

void rcUpdateCallback(){
    uas_winch.updateRCInput();
}

Ticker rc_update(rcUpdateCallback, RC_DELTA_T, 0);



void setup(){
    uas_winch = Winch();
    rc_update.start();
}

void loop(){
    rc_update.update();
}
