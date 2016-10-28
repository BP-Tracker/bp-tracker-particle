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

  int nextSlot = (publishEventFront + publishEventCount)
    % PUBLISH_EVENT_BUFFER_SIZE;

  publish_event_t *p = &publishEventQueue[nextSlot];
  p->event = event;
  p->deviceNum = forDeviceNum;
  p->datetime = Time.now();
  strcpy(p->data, data);

  // find a free slot to place ack event
  if(ackRequired){
    bool foundSlot = false;

    for(int i = 0; i < ACK_EVENT_BUFFER_SIZE; i++){
      ack_event_t *t = &ackEventBag[i];

      if(!t->ackNotReceived){ // ack received so not needed anymore

        memset(t, 0, sizeof(ack_event_t));

        publish_event_t *p2 = &(t->publishEvent);
        p2->event = event;
        p2->deviceNum = forDeviceNum;
        p2->datetime = Time.now();
        strcpy(p2->data, data);

        t->publishCount = 1;
        t->lastPublish = Time.now();

        ackEventCount++; //TODO should this be atomic with the next line?
        t->ackNotReceived = true;
        foundSlot = true;
      }
    }
    if(!foundSlot){ // should never get here
      Serial.println("ERROR: could not find ACK slot");
      return false;
    }
  }

  publishEventCount++;

  return true;
}

// look for events that need to be resubmitted
int BPT_Controller::_processAckEvent(){
  if(!ackEventCount){
    return 0;
  }

  //TODO





}

int BPT_Controller::_processPublishEvent(){

  if(!publishEventCount){
    return 0;
  }

  int published = 0;
  while(published < MAX_SEQUENTIAL_PUBLISH && publishEventCount > 0){
    publish_event_t *t = &publishEventQueue[publishEventFront];

    Serial.printf("publishing event: %u\n [deviceNum=%u][data=%s]",
      t->event, t->deviceNum, t->data);

    /*
    Particle.publish("bpt:event",
        String::format("%u,%s", t->event, t->data), 60, PRIVATE);
    */
    published++;
    publishEventCount--;
    publishEventFront = (publishEventFront + 1) % PUBLISH_EVENT_BUFFER_SIZE;
  }
  return published;
}
