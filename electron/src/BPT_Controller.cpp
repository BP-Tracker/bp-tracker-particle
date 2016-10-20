#include "BPT_Controller.h"

BPT_Controller::BPT_Controller(application_ctx_t *applicationCtx)
  : BPT(applicationCtx),
  #ifdef EXTERNAL_DEVICE_MT3339
    gpsModule(BPT_GPS_MT3339(applicationCtx)),
  #else
    gpsModule(BPT_GPS(applicationCtx)),
  #endif
    mode(CONTROLLER_MODE_NORMAL),
    state(STATE_INIT) {
}

bool BPT_Controller::getGpsCoord(gps_coord_t *c){
  return gpsModule.getGpsCoord(c);
}

void BPT_Controller::setup(void) {

  //TODO: check result
  registerProperty(PROP_CONTROLLER_MODE, this);

  mode = getProperty(PROP_CONTROLLER_MODE, CONTROLLER_MODE_NORMAL);
  state = STATE_INIT;

  if (EXTERNAL_DEVICE_MT3339){
    gpsModule.init( &(applicationCtx->devices[EXTERNAL_DEVICE_MT3339]) );
  }else{
    gpsModule.init();
  }


  //TODO: check return
  gpsModule.enable();



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

// main loop
void BPT_Controller::loop(void) {

  gpsModule.update();


}



//TODO: complete
void BPT_Controller::reset(void) {

}

bool BPT_Controller::setState(controller_state_t s){
  state = s;
  return true;
}

controller_state_t BPT_Controller::getState(){
  return state;
}

bool BPT_Controller::setMode(controller_mode_t m){
  mode = m;
  return true;
}

controller_mode_t BPT_Controller::getMode(void){
  return mode;
}
