#include "BPT_Controller.h"

BPT_Controller::BPT_Controller(application_ctx_t *a)
  : gps(BPT_GPS(a)),
    applicationCtx(a),
    controllerMode(CONTROLLER_MODE_NORMAL),
    systemState(STATE_OFFLINE) {
}

void BPT_Controller::setup(void) {

}

//TODO: complete
void BPT_Controller::reset(void) {

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
