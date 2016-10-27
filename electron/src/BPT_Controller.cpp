#include "BPT_Controller.h"

BPT_Controller::BPT_Controller(application_ctx_t *applicationCtx)
  : BPT(applicationCtx),
  #ifdef EXTERNAL_DEVICE_MT3339
    gpsModule(BPT_GPS_MT3339(applicationCtx)),
  #else
    gpsModule(BPT_GPS(applicationCtx)),
  #endif
  #ifdef EXTERNAL_DEVICE_LIS3DH
    accelModule(BPT_Accel_LIS3DH(applicationCtx)),
  #else
    accelModule(BPT_Accel(applicationCtx)),
  #endif
    cMode(CONTROLLER_MODE_NORMAL),
    cState(STATE_INIT) {

	//TODO: initialize buffers

}

bool BPT_Controller::getGpsCoord(gps_coord_t *c){
  return gpsModule.getGpsCoord(c);
}

int BPT_Controller::getAcceleration(accel_t *t){
  return accelModule.getAcceleration(t);
}

bool BPT_Controller::receive(gps_coord_t *coord, uint8_t deviceNumber){
  int i = (remoteGpsIndex + 1) % MAX_REMOTE_GPS_COORDS;
  remote_gps_coord_t c = remoteGpsCoord[i];

  memset(&c, 0, sizeof(remote_gps_coord_t)); // clears the data

  c.datetime = Time.now();
  c.coord.lat = coord->lat;
  c.coord.lon = coord->lon;
  c.device = deviceNumber;

  remoteGpsIndex = i;

  return true;
}

bool BPT_Controller::receive(application_event_t e, const char *data,
	uint8_t deviceNumber){
	// TODO

	return true;
}

void BPT_Controller::setup(void) {

  //TODO: check result
  registerProperty(PROP_CONTROLLER_MODE, this);

  cMode = getProperty(PROP_CONTROLLER_MODE, CONTROLLER_MODE_NORMAL);
  cState = STATE_INIT;

  #ifdef EXTERNAL_DEVICE_MT3339
    gpsModule.init( &(applicationCtx->devices[EXTERNAL_DEVICE_MT3339]) );
  #else
    gpsModule.init();
  #endif

  #ifdef EXTERNAL_DEVICE_LIS3DH
    accelModule.init( &(applicationCtx->devices[EXTERNAL_DEVICE_LIS3DH]) );
  #else
    accelModule.init();
  #endif


  //TODO: check return
  gpsModule.enable();
  accelModule.enable();
}

// main controller logic
void BPT_Controller::loop(void) {

  gpsModule.update();
  accelModule.update();

	if(publishEventCount >= PUBLISH_EVENT_BUFFER_SIZE - 1
	 	|| ackEventCount >= ACK_EVENT_BUFFER_SIZE - 1){
		// The buffers are full, allow the controller time
		// to process them

		//TODO
	 }

}



//TODO: complete
void BPT_Controller::reset(void) {

}

bool BPT_Controller::setState(controller_state_t s){
  cState = s;
  return true; // return true if the state was changed
}

controller_state_t BPT_Controller::getState(){
  return cState;
}

bool BPT_Controller::setMode(controller_mode_t m){
  cMode = m;
  return true;
}

controller_mode_t BPT_Controller::getMode(void){
  return cMode;
}


bool BPT_Controller::publish(application_event_t event,
		const char *data, bool ackRequired,
		uint8_t forDeviceNum, int _numOfFreeSlots){

	if( ( ackRequired &&
				ackEventCount > (ACK_EVENT_BUFFER_SIZE - _numOfFreeSlots)
			) || ( !ackRequired && publishEventCount
								> (PUBLISH_EVENT_BUFFER_SIZE - _numOfFreeSlots) ) ){
		return false; // no room
	}

	return false;
}

int BPT_Controller::_processPublishEvent(){
	return 0;
}
