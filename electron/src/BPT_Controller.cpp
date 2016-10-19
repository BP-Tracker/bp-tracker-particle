#include "BPT_Controller.h"

BPT_Controller::BPT_Controller(application_ctx_t *applicationCtx)
  : BPT(applicationCtx),
  #ifdef EXTERNAL_DEVICE_MT3339
    gpsModule(BPT_GPS_MT3339(applicationCtx)),
  #else
    gpsModule(BPT_GPS(applicationCtx)),
  #endif
    controllerMode(CONTROLLER_MODE_NORMAL),
    systemState(STATE_OFFLINE) {
}

void BPT_Controller::setup(void) {

  systemState = STATE_INIT;

  // state = STATE_INIT;
  //
  // sys.init();
  // gps.init();
  // //accel.init();
  // //pmon.init();
  //
  // bool enableSuccess = gps.enable();
  //
  // if(!enableSuccess){
  //   // TODO: .....
  //
  // }

}

//TODO: complete
void BPT_Controller::reset(void) {

}

system_state_t BPT_Controller::getState(){
  return systemState;
  //return applicationCtx->state;
}

// main loop
void BPT_Controller::loop(void) {

}


bool BPT_Controller::setMode(controller_mode_t mode){
  controllerMode = mode;
  return true;
}

controller_mode_t BPT_Controller::getMode(void){
  return controllerMode;
}
