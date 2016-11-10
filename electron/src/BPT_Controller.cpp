#include "BPT_Controller.h"

#define TIME_DELTA(t) (int)(millis() - (t))
#define TIME_DELTA_SEC(t) (int)((millis() - (t)) / 1000)

BPT_Controller::BPT_Controller(application_ctx_t *applicationCtx)
  : BPT(applicationCtx),
    gpsModule(BPT_GPS(applicationCtx)),
    accelModule(BPT_Accel(applicationCtx)),
    publishEventCount(0),
    ackEventCount(0),
    cMode(CONTROLLER_MODE_NORMAL),
    cState(STATE_ONLINE_WAIT),
    pState(STATE_ONLINE_WAIT),
    _stateTime(millis()),
    _publishTime(millis()),
    _publishAckTime(millis()),
    _ackEventsEnabled(false),
    _hasException(false),
    _publishEventFront(0),
    _remoteGpsIndex(0),
    _requestGpsSent(false),
    _geoFenceRadius(0),
    _wakeDetectionConfigured(false),
    _resumePreviousState(STATE_ACTIVATED),
    _panicGpsPublishEventCount(0),
    _lastRemoteCommunication(0),
    _lastRemoteDeviceNum(0),
    _softPanicStartTime(-1) {

  //TODO: initialize buffers
}

void BPT_Controller::setup(void) {

  //TODO: check result
  registerProperty(PROP_CONTROLLER_MODE, this);
  registerProperty(PROP_ACK_ENABLED, this);
  registerProperty(PROP_GEOFENCE_RADIUS, this);

  _ackEventsEnabled = getProperty(PROP_ACK_ENABLED, true);
  _geoFenceRadius = getProperty(PROP_GEOFENCE_RADIUS, DEFAULT_GEOFENCE_RADIUS);
  cMode = getProperty(PROP_CONTROLLER_MODE, CONTROLLER_MODE_TEST); //TODO: change later
  cState = STATE_ONLINE_WAIT;

  applicationCtx->mode = cMode;

  gpsModule.init();
  accelModule.init();

  //TODO: check return
  gpsModule.enable();
  accelModule.enable();
}

// main logic
void BPT_Controller::loop(void) { //TODO

  gpsModule.update();
  accelModule.update();

  _checkPublishQueue();
  _checkAckQueue();

  //NB: the controller should always have room for one publish slot
  if( ackEventCount >= ACK_EVENT_BUFFER_SIZE
      || publishEventCount >= PUBLISH_EVENT_BUFFER_SIZE){

    if(Particle.connected()
      && TIME_DELTA_SEC(_stateTime) >= CONTROLLER_ERROR_TIMEOUT ){

      _logException("Buffer stall event occured");
      _resetTime(&_stateTime);
    }

    return; // allow time for the buffers to process
  }

  int now = Time.now();

  switch(cState){
    case STATE_ONLINE_WAIT:
      if(Particle.connected()){
        setState(STATE_RESET, true);
      }
      break;

    case STATE_RESET:
      // reset accel module
      // reset gps module??
      // check if accel module was activated TODO
      // setState(SLEEP_STATE, true)
      _requestGpsSent = false;
      _panicGpsPublishEventCount = 0;
      _softPanicStartTime = -1;
      _wakeDetectionConfigured = false;

      setState(STATE_RESET_WAIT, true);
      break;

    case STATE_RESET_WAIT:
      if(TIME_DELTA(_stateTime) >= RESET_WAIT_STATE_DELAY){

        if( applicationCtx->mode == CONTROLLER_MODE_TEST
            && pState != STATE_RESUMED){ // in TEST_MODE go to paused state
          setState(STATE_PAUSED, true);
        }else{
          setState(STATE_ACTIVATED, true);
        }

        break;
      }
      break;
    case STATE_ACTIVATED: //find out what state we should be in
      {
        remote_gps_coord_t *rGps = &_remoteGpsCoord[_remoteGpsIndex];

        //Serial.printf("controller: get remote GPS [date=%i] [now=%i] [lat=%f][lon=%f]\n",
        //  rGps->datetime, now, rGps->coord.lat, rGps->coord.lon );

        if( (now - rGps->datetime) > GPS_COORD_MAX_AGE ){ // request GPS data

          if(!_requestGpsSent ){
            _requestGpsSent = publish(EVENT_REQUEST_GPS, "", true);
          }

          if( TIME_DELTA_SEC(_stateTime) >= REQUEST_GPS_TIMEOUT ){ // timeout

            if(setState(STATE_SOFT_PANIC, true, 2)){ //TODO: send state change?
                publish(EVENT_SOFT_PANIC, ""); // TODO: sent GPS coord, todo: ack required???
                _requestGpsSent = false;
            }
          }

        }else{ // check the device is within a geofence

          //TODO
          gps_coord_t cGps;
          // TODO: is okay here to use the last known GPS position?
          // TODO: do something with the age of this coord
          int r = gpsModule.getGpsCoord(&cGps, true);

          if( r < 0 ){ // no GPS

              if( TIME_DELTA_SEC(_stateTime) >= GPS_ACQUISITION_TIMEOUT ){
                if(setState(STATE_OFFLINE, true, 2)){
                  publish(EVENT_NO_GPS_SIGNAL, ""); //TODO: can we setup a HW interrupt?
                  _requestGpsSent = false;
                };
              }
          }

          float distance = gpsModule.getDistanceTo(&(rGps->coord));

          Serial.printf("controller: distance to GPS coord [%f][%f] [d=%f]\n",
            cGps.lat, cGps.lon, distance );

          if(distance <= _geoFenceRadius){ // in geofence
            setState(STATE_DISARMED, true);
            _requestGpsSent = false;

          }else if( accelModule.isMoving() > 0 ){
            if(setState(STATE_PANIC, true, 2)){ // panic mode
              _requestGpsSent = false;
              char temp[64];
              snprintf(temp, sizeof(temp), "%f,%f", cGps.lat, cGps.lon);
              publish(EVENT_PANIC, temp); //TODO: ack required?
            }
          }else{ // disarm mode TODO: is this right?
            setState(STATE_DISARMED, true);
            _requestGpsSent = false;
          }
        }
      }
      break; // end of actived state
    case STATE_DEACTIVATED:

      // clears any pending publish events and waits here indefinitely
      // for a state change.

      if(pState != STATE_DEACTIVATED){
         // reset all pertinent state variables because the controller can
         // jump to any of the pubic states
         _requestGpsSent = false;
         _panicGpsPublishEventCount = 0;
         _softPanicStartTime = -1;
         _wakeDetectionConfigured = false;

         for(int i = 0; i < ACK_EVENT_BUFFER_SIZE; i++){
           ack_event_t *aE =  &_ackEventBag[i];
           aE->ackNotReceived = false; // clear slot
         }
         ackEventCount = 0;
         publishEventCount = 0;
         _publishEventFront = 0;

         setState(STATE_DEACTIVATED, true, 0, false);
      }

      break;

    case STATE_OFFLINE:
      // TODO
      break;
    case STATE_DISARMED:

      if(!_wakeDetectionConfigured){
        if(!accelModule.reset()){
          Serial.println("controller: warning - could not reset accelerometer");
        }else{
          _wakeDetectionConfigured = true;
        }
      }

      if( TIME_DELTA_SEC(_stateTime) >= AUTO_ARM_AFTER_IDLE_PERIOD ){
        if( accelModule.hasMoved( true ) ){ // device has moved within wait period

          _resetTime(&_stateTime);
        }else{
          _wakeDetectionConfigured = false;
          setState(STATE_ARMED, true);
        }
      }


      break; // end STATE_DISARMED
    case STATE_ARMED:

      if( accelModule.hasMoved() ){
        setState(STATE_ACTIVATED, true, 0);
      }else{
        if( SLEEP_STATE_PERIOD > 0 ){
            setState(STATE_SLEEP, true);
        }
      }
      break;

    case STATE_SLEEP: // state disabled when SLEEP_STATE_PERIOD = 0

      // don't sleep in certain controller modes and when events are pending
      if( SLEEP_STATE_PERIOD > 0
            && ackEventCount == 0
            && publishEventCount == 0
            && applicationCtx->mode != CONTROLLER_MODE_HIGH_SPEED ){

        // go to sleep and wakeup on WKP or scheduled time
      	System.sleep(WKP, RISING, SLEEP_STATE_PERIOD, SLEEP_NETWORK_STANDBY);

        // from WakeOnMovement.cpp
        // This delay should not be necessary, but sometimes things don't seem to work right
        // immediately coming out of sleep.
    		delay(500);
      }

      setState(STATE_ARMED, true, 0); // don't publish this transition
      break;
    case STATE_SOFT_PANIC:

      if(_softPanicStartTime < 0){
        _softPanicStartTime = Time.now();
      }

      if(_lastRemoteCommunication >= _softPanicStartTime){

        // stay here until we get any response from the device
        // via bpt:probe or bpt:ack events.

        setState(STATE_ACTIVATED, true); //TODO: go to STATE_RESET instead?
      }else{
        if(SOFT_PANIC_TO_OFFLINE_PERIOD > 0
            && TIME_DELTA_SEC(_stateTime) >= SOFT_PANIC_TO_OFFLINE_PERIOD){
          setState(STATE_OFFLINE, true);
        }
      }

      break;

    case STATE_PANIC:

      if(PANIC_GPS_PUBLISH_FREQUENCY > 0
          && _panicGpsPublishEventCount < MAX_PANIC_GPS_PUBLISH_EVENTS
          && TIME_DELTA_SEC(_stateTime) >= PANIC_GPS_PUBLISH_FREQUENCY  ){

        gps_coord_t pGps;
        if( gpsModule.getGpsCoord(&pGps) >= 0 ){ //FIXME: what about case of no GPS signal???

          char temp[64];
          snprintf(temp, sizeof(temp), "%f,%f", pGps.lat, pGps.lon);

            if(publish(EVENT_PANIC, temp)){
              _resetTime(&_stateTime);
              _panicGpsPublishEventCount++;
            }
        }
      }else{

        if(PANIC_TO_OFFLINE_PERIOD > 0
            && TIME_DELTA_SEC(_stateTime) >= PANIC_TO_OFFLINE_PERIOD){
          setState(STATE_OFFLINE, true);
        }

      }

      break;

    case STATE_PAUSED: // do nothing: state triggered externally

      //FIXME: the TIME_DELTA on _stateTime should not change

      if(pState != STATE_PAUSED){
        _resumePreviousState = pState;
        setState(STATE_PAUSED, true, 0); // so pState == STATE_PAUSED
      }

      if( MAX_PAUSED_STATE_PERIOD != 0
          && TIME_DELTA_SEC(_stateTime) >= MAX_PAUSED_STATE_PERIOD ){
        setState(STATE_RESUMED, true);
      }
      break;
    case STATE_RESUMED:

      if(pState == STATE_PAUSED){
        setState(_resumePreviousState, true, 0); // don't publish this state change
      }else if(pState == STATE_DEACTIVATED){
        setState(STATE_ACTIVATED, true, 0);
      }else{
        setState(pState, true, 0); // got to state without going through pasued
      }

      break;
  }
}

void BPT_Controller::_logException(const char *msg){ //TODO: print to serial?
  _hasException = true;
  strncpy(_exceptionMessage, msg, MAX_EXCEPTION_MSG_LENGTH);
}

bool BPT_Controller::hasException(){
  return _hasException;
}

const char* BPT_Controller::getException(bool reset){
  if(reset){
    _hasException = false;
    _exceptionMessage[0] = '\0';
  }
  return _exceptionMessage;
}

int BPT_Controller::getGpsCoord(gps_coord_t *c){
  return gpsModule.getGpsCoord(c);
}

int BPT_Controller::getAcceleration(accel_t *t){
  return accelModule.getAcceleration(t);
}

bool BPT_Controller::receive(gps_coord_t *coord, uint8_t deviceNumber){
  int i = (_remoteGpsIndex + 1) % MAX_REMOTE_GPS_COORDS;
  remote_gps_coord_t *c = &_remoteGpsCoord[i];

  memset(c, 0, sizeof(remote_gps_coord_t)); // clears the data

  c->datetime = Time.now();
  c->coord.lat = coord->lat;
  c->coord.lon = coord->lon;
  c->device = deviceNumber;

  _remoteGpsIndex = i;
  _lastRemoteCommunication = Time.now();
  _lastRemoteDeviceNum = deviceNumber;

  Serial.printf("controller: received remote GPS [lat=%f][lon=%f][date=%i]\n",
    c->coord.lat, c->coord.lon, c->datetime);

  return true;
}


bool BPT_Controller::receive(application_event_t e, const char *data,
  uint8_t deviceNumber){

  _lastRemoteCommunication = Time.now();
  _lastRemoteDeviceNum = deviceNumber;

  if(e == EVENT_PROBE_CONTROLLER){
    _probeController(deviceNumber);
    return true;
  }

  bool found = false;

  if(ackEventCount <= 0 || !_ackEventsEnabled){
    Serial.println("controller: warning event received but not waiting for any");
    return found;
  }

  for(int i = 0; i < ACK_EVENT_BUFFER_SIZE && !found; i++){
    ack_event_t *aE =  &_ackEventBag[i];
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

  if(e == EVENT_REQUEST_GPS){ //TODO: test this function
    // the ack should also contains gps coords, retrieve them
    String d = String(data);
    int sep = d.indexOf(",");
    if(sep > 0){
      String latS = d.substring(0, sep);
      String lonS = d.substring(sep + 1);

      gps_coord_t t = { (float)atof(latS), (float)atof(lonS) };
      receive(&t, deviceNumber);

    }else{
      Serial.println("controller: warning - EVENT_REQUEST_GPS does not contain GPS coords");
    }
  }

  return found;
}


void BPT_Controller::reset(void) { //TODO: complete
}

/*
  _force -> set to true to allow internal state changes
  _reserveSlots -> does the controller have this many publish slots available
    An EVENT_STATE_CHANGE event will be published for public states
    when _reserveSlots > 0
  _resetTime resets _stateTime

  returns false when a state change did not happen (not enough free slots) TODO
*/
bool BPT_Controller::setState(controller_state_t s, bool _force,
  int _reserveSlots, bool _resetStateTime){

  if( s == 0 || s > NUM_CONTROLLER_STATES ){
    Serial.printf("controller: warning - unknown state passed %i\n", s);
    return false;
  }

  //TODO
  /*
  if(!_force && s >= INTERNAL_STATES_INDEX){
    Serial.printf("controller: warning - cannot set internal state %i\n", s);
    return false;
  }
  */

  pState = cState;
  cState = s;

  if(_resetStateTime){
    _resetTime(&_stateTime);
  }
  return true;
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

// _numOfFreeSlots and _ackRetryNumber are private arguments
// used by the controller
// NB: _ackRetryNumber holds the ackRequired status (default=0) when ackRequired is false
bool BPT_Controller::publish(application_event_t event,
    const char *data, bool ackRequired,
    uint8_t forDeviceNum, int _numOfFreeSlots, int _ackRetryNumber){

  if( ( ackRequired &&
         ackEventCount > (ACK_EVENT_BUFFER_SIZE - _numOfFreeSlots)
       ) || ( !ackRequired && publishEventCount
           > (PUBLISH_EVENT_BUFFER_SIZE - _numOfFreeSlots) ) ){
    return false; // no room
  }

  //TODO: publish events differently when the controller mode is test

  int nextSlot = (_publishEventFront + publishEventCount)
    % PUBLISH_EVENT_BUFFER_SIZE;

  publish_event_t *p = &_publishEventQueue[nextSlot];
  p->event = event;
  p->deviceNum = forDeviceNum;
  p->datetime = Time.now();
  p->retryCount = ackRequired ? 1 : _ackRetryNumber;
  strcpy(p->data, data);

  // find a free slot to place ack event
  if(_ackEventsEnabled && ackRequired){
    bool foundSlot = false;

    for(int i = 0; i < ACK_EVENT_BUFFER_SIZE && !foundSlot; i++){
      ack_event_t *t = &_ackEventBag[i];

      if(!t->ackNotReceived){ // ack received so not needed anymore

        memset(t, 0, sizeof(ack_event_t));

        publish_event_t *p2 = &(t->publishEvent);
        p2->event = event;
        p2->deviceNum = forDeviceNum;
        p2->datetime = Time.now();
        p2->retryCount = 1; // ack required

        if(strlen(data) > MAX_PUBLISH_DATA_LEN ){
          _logException("Event data trucated to MAX_PUBLISH_DATA_LEN");
        }
        strncpy(p2->data, data, MAX_PUBLISH_DATA_LEN);

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

void BPT_Controller::_resetTime(unsigned long *t){
  *t = millis();
}

void BPT_Controller::_checkAckQueue(){

  if( _ackEventsEnabled && ackEventCount
    && TIME_DELTA(_publishAckTime) > CHECK_ACK_QUEUE_FREQUENCY){

    // process ack queue
    int c = _processAckEvent();
    if(c > 0){
      Serial.printf("controller: %i act events processed\n", c);
    }

    _resetTime(&_publishAckTime);
  }
}

// look for events that need to be resubmitted
int BPT_Controller::_processAckEvent(){ //TODO
  if(ackEventCount <= 0 || publishEventCount >= PUBLISH_EVENT_BUFFER_SIZE){
    return 0; // no space or events
  }

  bool done = false;
  int processed = 0;

  for(int i = 0; i < ACK_EVENT_BUFFER_SIZE && !done; i++){
    ack_event_t *aE =  &_ackEventBag[i];

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

        bool success = publish(pE->event, pE->data, false, pE->deviceNum, 1,
           aE->publishCount + 1);

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

void BPT_Controller::_checkPublishQueue(){

  if( publishEventCount
      && TIME_DELTA(_publishTime) > CHECK_PUBLISH_QUEUE_FREQUENCY){

    // process publish queue
    int c = _processPublishEvent();
    _resetTime(&_publishTime);

    if(c >= MAX_SEQUENTIAL_PUBLISH){   // add a cooldown to the time
      _publishTime = _publishTime
        + abs(SEQUENTIAL_PUBLISH_COOLDOWN - CHECK_PUBLISH_QUEUE_FREQUENCY);
    }

    Serial.printf("controller: %i events processed [t=%i][d=%i]\n",
      c, _publishTime, TIME_DELTA(_publishTime));
  }
}

int BPT_Controller::_processPublishEvent(){

  if(!publishEventCount){
    return 0;
  }

  if(!Particle.connected()){
    return 0;
  }

  int published = 0;
  while(published < MAX_SEQUENTIAL_PUBLISH && publishEventCount > 0){
    publish_event_t *t = &_publishEventQueue[_publishEventFront];

    /*
    Particle.publish("bpt:event",
        String::format("%u,%u,%s", t->ackRequired, t->event, t->data), 60, PRIVATE);
    */
    //TODO: check for correctness
    if(strlen(t->data) > 0){
      Serial.printf("PUBLISH[bpt:event~%u,%i,%s]\n", t->event, t->retryCount, t->data);
    }else{
      Serial.printf("PUBLISH[bpt:event~%u,%i]\n", t->event, t->retryCount);
    }

    Serial.printf("controller: published [event=%u][ack=%i][deviceNum=%u][data=%s]\n",
      t->event, t->retryCount, t->deviceNum, t->data);

    totalPublishedEvents++;
    published++;
    publishEventCount--;
    _publishEventFront = (_publishEventFront + 1) % PUBLISH_EVENT_BUFFER_SIZE;
  }
  return published;
}


int BPT_Controller::totalDroppedAckEvents = 0;
int BPT_Controller::totalPublishedEvents = 0;
