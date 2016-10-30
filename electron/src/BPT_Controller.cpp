#include "BPT_Controller.h"

#define TIME_DELTA(t) (int)(millis() - (t))

BPT_Controller::BPT_Controller(application_ctx_t *applicationCtx)
  : BPT(applicationCtx),
    gpsModule(BPT_GPS(applicationCtx)),
    accelModule(BPT_Accel(applicationCtx)),
    publishEventCount(0),
    ackEventCount(0),
    cMode(CONTROLLER_MODE_NORMAL),
    cState(STATE_INIT),
    pState(STATE_INIT),
    controllerStateTime(millis()),
    publishTime(millis()),
    publishAckTime(millis()) {

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

// this is a special event a remote device can send to
// wake up the controller and perform any maintenance duties
void BPT_Controller::_probeController(uint8_t deviceNumber){ //TODO: complete the logic
  bool s = publish(EVENT_PROBE_CONTROLLER,
    String::format("%u", deviceNumber), false, deviceNumber);

  if(!s){
      Serial.println("controller: warning cannot publish EVENT_PROBE_CONTROLLER event");
  }

  //TODO: wakeup any pending ack events for the device
}

bool BPT_Controller::receive(application_event_t e, const char *data,
  uint8_t deviceNumber){

  if(e == EVENT_PROBE_CONTROLLER){
    _probeController(deviceNumber);
    return true;
  }

  bool found = false;

  if(ackEventCount <= 0 || !ackEventsEnabled){
    Serial.println("controller: warning event received but not waiting for any");
    return found;
  }

  for(int i = 0; i < ACK_EVENT_BUFFER_SIZE && !found; i++){
    ack_event_t *aE =  &ackEventBag[i];
    publish_event_t *pE = &(aE->publishEvent);
    bool needSpecifcDev = pE->deviceNum != 0;

    if(aE->ackNotReceived && e == pE->event){

      if(!needSpecifcDev || pE->deviceNum == deviceNumber){
        aE->ackNotReceived = false;
        ackEventCount--;
        found = true;
      }
    }
  }
  return found;
}

void BPT_Controller::setup(void) {

  //TODO: check result
  registerProperty(PROP_CONTROLLER_MODE, this);
  registerProperty(PROP_ACK_ENABLED, this);

  ackEventsEnabled = getProperty(PROP_ACK_ENABLED, true);
  cMode = getProperty(PROP_CONTROLLER_MODE, CONTROLLER_MODE_NORMAL);
  cState = STATE_INIT;

  gpsModule.init();
  accelModule.init();

  //TODO: check return
  gpsModule.enable();
  accelModule.enable();
}

// main controller logic
void BPT_Controller::loop(void) { //TODO

  gpsModule.update();
  accelModule.update();

  // process publish queue
  if( publishEventCount
      && TIME_DELTA(publishTime) > CHECK_PUBLISH_QUEUE_FREQUENCY){

    int c = _processPublishEvent();
    _resetTime(&publishTime);

    if(c >= MAX_SEQUENTIAL_PUBLISH){   // add a cooldown to the time
      publishTime = publishTime
        + abs(SEQUENTIAL_PUBLISH_COOLDOWN - CHECK_PUBLISH_QUEUE_FREQUENCY);
    }

    Serial.printf("controller: %i events processed [t=%i][d=%i]\n",
      c, publishTime, TIME_DELTA(publishTime));
  }

  // process ack queue
  if( ackEventsEnabled && ackEventCount
    && TIME_DELTA(publishAckTime) > CHECK_ACK_QUEUE_FREQUENCY){

    int c = _processAckEvent();
    if(c > 0){
      Serial.printf("controller: %i act events processed\n", c);
    }

    _resetTime(&publishAckTime);
  }

  // run states TODO

}

void BPT_Controller::_resetTime(unsigned long *t){
  *t = millis();
}

void BPT_Controller::reset(void) { //TODO: complete
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
  // TODO: check if mode can be set
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
  if(ackEventsEnabled && ackRequired){
    bool foundSlot = false;

    for(int i = 0; i < ACK_EVENT_BUFFER_SIZE && !foundSlot; i++){
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
      Serial.println("controller: ERROR - could not find ACK slot");
      return false;
    }
  }

  publishEventCount++;

  return true;
}

// look for events that need to be resubmitted
int BPT_Controller::_processAckEvent(){ //TODO
  if(ackEventCount <= 0 || publishEventCount >= PUBLISH_EVENT_BUFFER_SIZE){
    return 0; // no space or events
  }

  bool done = false;
  int processed = 0;

  for(int i = 0; i < ACK_EVENT_BUFFER_SIZE && !done; i++){
    ack_event_t *aE =  &ackEventBag[i];

    if(aE->publishCount > MAX_ACK_EVENT_RETRY && aE->publishFailure == false ){
        aE->publishFailure = true;
        aE->ackNotReceived = false; //TODO: handle this differently?
        totalDroppedAckEvents++;
        ackEventCount--;

        if(ackEventCount < 0){
          Serial.println("controller: ackEventCount < 0!");
          ackEventCount = 0; // safe guard is this necessary?
        }
    }

    if( aE->ackNotReceived && !aE->publishFailure ){
      // is it time to republish?
      int delaySec = pow(ACK_EVENT_RETRY_DELAY, aE->publishCount);
      int now = Time.now();

      Serial.printf("controller: [delaySec=%i][now=%i][lp=%i]\n",
        delaySec, now, aE->lastPublish  );

      if( (aE->lastPublish + delaySec) <= now ){
        publish_event_t *pE = &(aE->publishEvent);

        bool success = publish(pE->event, pE->data, false, pE->deviceNum);
        if(success){

          aE->publishCount = aE->publishCount + 1;
          aE->lastPublish = now;
          processed++;

        }else{
          done = true; // no more free slots
        }
      }
    }
  }
  return processed;
}

int BPT_Controller::_processPublishEvent(){

  if(!publishEventCount){
    return 0;
  }

  int published = 0;
  while(published < MAX_SEQUENTIAL_PUBLISH && publishEventCount > 0){
    publish_event_t *t = &publishEventQueue[publishEventFront];

    /*
    Particle.publish("bpt:event",
        String::format("%u,%s", t->event, t->data), 60, PRIVATE);
    */

    Serial.printf("controller: published [event=%u][deviceNum=%u][data=%s]\n",
      t->event, t->deviceNum, t->data);

    //TODO: remove later
    //int temp = totalPublishedEvents % PUBLISH_EVENT_BUFFER_SIZE;
    //publishTest[temp] = millis();

    totalPublishedEvents++;
    published++;
    publishEventCount--;
    publishEventFront = (publishEventFront + 1) % PUBLISH_EVENT_BUFFER_SIZE;
  }
  return published;
}


int BPT_Controller::totalDroppedAckEvents = 0;
int BPT_Controller::totalPublishedEvents = 0;
