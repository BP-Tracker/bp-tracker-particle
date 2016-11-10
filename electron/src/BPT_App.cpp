//#include "Particle_AssetTracker.h"
#include "BPT.h"
#include "BPT_GPS.h"
#include "BPT_Controller.h"

/*********************************************
  Constants
*********************************************/
int ON_BOARD_LED = D7;
const int SERIAL_COMMAND_BUFFER_SIZE = 64;

//SYSTEM_THREAD(ENABLED); //TODO: is this required?

//TODO
//PRODUCT_ID(1);
//PRODUCT_VERSION(1);

/*********************************************
  Application variables
*********************************************/
extern external_device_t devices[EXTERNAL_DEVICE_COUNT];
application_ctx_t appCtx;
gps_coord_t gpsCoord;
unsigned long stateTime = 0;
int publishCount = 0;
int temp = 0;

char serialBuffer[SERIAL_COMMAND_BUFFER_SIZE];
int serialBufferIndex = 0;
bool serialLatch = true;

BPT_Controller controller = BPT_Controller(&appCtx);
FuelGauge fuelGauge;

//NUM_CLOUD_EVENTS, CLOUD_EVENTS and CLOUD_FUNCS are declared at the bottom



/*********************************************
  Functions
*********************************************/

/**
 * Publishes the event using Particle.publish and Serial functions
 * @param name The registered event name
 * @param data  The data
 */
void publish(String name, const char *data){
  //Particle.publish(name, data, 60, PRIVATE); //TODO
  Serial.printf("PUBLISH[%s~%s]\n", name.c_str(), data);
}


// override
void loop(){
  controller.loop();

  if(controller.hasException()){
    Serial.printf("app: controller exception: %s\n", controller.getException(true) );
  }

  if (millis() - stateTime > 10000) {
    stateTime = millis();


    //for(int i = 0; i < SERIAL_COMMAND_BUFFER_SIZE; i++){
    //  Serial.print(serialBuffer[i]);
    //}
   //Serial.println();



    Serial.printf("app: [state=%u][mode=%u][publishEvent=%u]",
      controller.getState(), controller.getMode(), controller.publishEventCount);

    Serial.printf("[ackEvent=%u][total=%i][dropped=%i]\n",
      controller.ackEventCount, controller.totalPublishedEvents,
      controller.totalDroppedAckEvents);
  }

}

/*
  Returns the device number from the command (if exists) and
  sets the index to the start of the data. This function
  returns 1 when no device number is found.
*/
int _getDeviceNumber(String command, int *commandStartIndex){
  int deviceId = 1; // the default

  int deviceSep = command.indexOf(":");
  if(deviceSep > 0){ // device number was passed in the command
    *commandStartIndex = deviceSep + 1;
    deviceId = atoi(command.substring(0, deviceSep));
  }else{
    *commandStartIndex = 0;
  }

  return deviceId;
}


// get or set the current state of the controller (controller_state_t type)
// If setting the state, the return 0 if the state was changed, -1 otherwise
// format [deviceNum:]controller_state_t
int stateFn(String command){
  controller_state_t s = controller.getState();
  Serial.printf("app: stateFn - [state=%u]", s);

  int result = 0;
  int commandIndex = 0;
  _getDeviceNumber(command, &commandIndex);

  if(command.length() > 0 && commandIndex >= 0){ // set state
    int e = atoi(command.substring(commandIndex));
    controller_state_t newState = static_cast<controller_state_t>(e);

    result = controller.setState(newState) ? 0 : -1;
  }else{ // get state
    publish("bpt:state", String::format("%u", s));
  }

  return result;
}

// returns the current state of the controller including
// gps location and battery status
// format: "controller_mode_t,controller_state_t,batt(%),satellite,lat,lon"
// TODO: thread safe?
int getStatusFn(String command){
  controller_mode_t m = controller.getMode();
  controller_state_t s = controller.getState();
  int r = controller.getGpsCoord(&gpsCoord);

  publish("bpt:status", String::format("%u,%u,%.2f,%i,%f,%f", m, s,
      fuelGauge.getSoC(), r, gpsCoord.lat, gpsCoord.lon));

    return 0;
}



// pass the coordinate to the controller for processing
int _processRemoteGpsCoord(float lat, float lon, int devNum){
  gps_coord_t coord;
  coord.lat = lat;
  coord.lon = lon;

  Serial.printf("app: processRemoteGpsCoord - [lat=%f][lon=%f][devNum=%u]\n",
    coord.lat, coord.lon, devNum);

  bool r = controller.receive(&coord, devNum);
  return r == true ? 1 : 0;
}

/*
  This function has a dual purpose:
  1 - when GPS coords are passed in, the controller will use it to
  determine the proximity of the remote device and update its
  state accordingly. This is used when the controller publishes a request
  to remote devices.

  2 - otherwise, it returns the age of the gps signal in seconds
  and publishes the coords of the device in the format:
  latitude,longitude

  curl https://api.particle.io/v1/devices/Lippy/bpt:gps
   -d access_token=${particle token list}

  TODO: is this thread safe?
*/
// accepts GPS coordinates in the format: "[deviceNum:]latitude,lonitude"
// TODO: support to send gps automatically every specified interval
int gpsCoordFn(String command){

  int commandIndex = 0;
  int sep = command.lastIndexOf(",");

  int deviceNum = _getDeviceNumber(command, &commandIndex);

  if(sep > 0){ // found GPS coords
    String latS = command.substring(commandIndex, sep);
    String lonS = command.substring(sep + 1);
    return _processRemoteGpsCoord(atof(latS), atof(lonS), deviceNum);
  }

  int r = controller.getGpsCoord(&gpsCoord);
  float lat = r < 0 ? 0 : gpsCoord.lat;
  float lon = r < 0 ? 0 : gpsCoord.lon;

  Serial.printf("app: gpsCoordFn - [status=%u][lat=%f][lon=%f]", r, lat, lon);

  if(r >= 0){
    publish("bpt:gps", String::format("%f,%f", lat, lon));
  }else{
    publish("bpt:event", String::format("%u", EVENT_NO_GPS_SIGNAL));
  }
  return r;
}

// publishes more information about the state of controller for troubleshooting
// or diagnostic purposes
// command : number -> publish the data to the cloud
int getDiagnosticFn(String command){ // TODO

  //TODO: input validation
  int publishToCloud = atoi(command);


  /*
  uint32_t freeMem = System.freeMemory();
  int firmwareVers = System.versionNumber();
  unsigned long runTime = millis();
  size_t eepromLen = EEPROM.length();
  */
  accel_t a;
  uint32_t freeMem = System.freeMemory();
  uint16_t status = controller.accelModule.mod_status.status;
  bool s = controller.accelModule.getStatus(MOD_STATUS_INTERRUPT);
  controller.accelModule.getAcceleration(&a);
  int mag = controller.accelModule.getMagnitude(&a);

  Serial.print("app: getDiagnosticFn - ");
  Serial.printf(
    "[status=%u][x=%f][y=%f][z=%f][m=%i][mem=%i][it=%i]\n",
    status, a.x, a.y, a.z, mag, freeMem, s == true ? 1 : 0);

  if(publishToCloud){
    publish("bpt:diag",
      String::format("%u,%f,%f,%f,%i,%u", status, a.x, a.y, a.z, mag, freeMem) );
  }

  return 1;
}

/*
  Registers a remote device as a candidate to respond to this controller.
  This is optional when only one remote device will be used.
*/
int registerRemoteDeviceFn(String command){
  //TODO
  return -1;
}


/*
  Probes the controller and returns a bpt:event EVENT_PROBE_CONTROLLER
  event if it can (the probe can also be triggered by sending a
  bpt:ack event). Command format: [deviceId:]
  TODO: probe application properties?
*/
int probeControllerFn(String command){
  int commandIndex = 0;
  int deviceNum = _getDeviceNumber(command, &commandIndex);

  Serial.printf("app: probeControllerFn - [devNum=%u]\n", deviceNum);

  bool r = controller.receive(EVENT_PROBE_CONTROLLER, "", deviceNum);
  return r == true ? 1 : -1;
}

// Sends an event acknowledgment to the controller
// NB: not all "btp:event" events require an acknowledgment
// format [deviceId:]application_event_t[,data1,data2,..]
int ackEventFn(String command){

  int commandIndex = 0;
  int deviceNum = _getDeviceNumber(command, &commandIndex);
  int sep = command.indexOf(",");

  String e = sep > 0 ?
    command.substring(commandIndex, sep): command.substring(commandIndex);
  application_event_t event = static_cast<application_event_t>(atoi(e));

  String d = sep > 0 ? command.substring(sep + 1): "";
  const char *data = d.c_str();

  Serial.printf("app: ackEventFn - [devNum=%u][event=%u][data=%s]\n",
     deviceNum, event, data);

  bool r = controller.receive(event, data, deviceNum);

  return r == true ? 1 : -1;
}

// sets up test routines for the controller and modules
// the data becomes effective once the controller mode is CONTROLLER_MODE_TEST
// returns 1 if the command was successful
// format: <test_input_1>[,data1,data2]
//TODO: input validation
int testInputFn(String command){

  if(command.length() <= 0 ){
    Serial.println("app: testInputFn - test_input_1 type is missing");
    return 0;
  }

  int sep = command.indexOf(",");
  String typeStr = command.substring(0, sep);
  test_input_t type = static_cast<test_input_t>(atoi(typeStr));

  if(type == TEST_INPUT_GPS && sep > 0 ){

    int dataSep = command.indexOf(",", sep + 1);
    String latS = command.substring(sep + 1, dataSep);
    String lonS = command.substring(dataSep + 1);

    gps_coord_t t = { (float)atof(latS), (float)atof(lonS) };
    controller.gpsModule.setTestData(&t);

  }else if(type == TEST_INPUT_AUTO_GPS){
    gps_coord_t t = { 0.0f, 0.0f }; //TODO: set more sensible auto gps coords
    controller.gpsModule.setTestData(&t); //TODO reset?

  }else if(type == TEST_INPUT_ACCEL_INT){ // default wake=1 when no data

    int wakeMode =  sep > 0  ? atoi( command.substring(sep) ) : 1;
    controller.accelModule.setTestData(wakeMode); //TODO reset?

  }else{
    Serial.printf("app: testInputFn - incorrect format for test_input_t: %u\n", type);
    return 0;
  }

  return 1;
}


//NB: events maps to cloudFns variable
int NUM_CLOUD_EVENTS = 8;
String const CLOUD_EVENTS[] = {
  "bpt:state",
  "bpt:gps",
  "bpt:status",
  "bpt:diag",
  "bpt:register",
  "bpt:ack",
  "bpt:probe",
  "bpt:test"
};
int (* const CLOUD_FUNCS[])(String) = {
  stateFn,
  gpsCoordFn,
  getStatusFn,
  getDiagnosticFn,
  registerRemoteDeviceFn,
  ackEventFn,
  probeControllerFn,
  testInputFn
};


void _processSerialCommand(String event, String command){
  Serial.printf("app: _processSerialCommand - [%s][%s]\n",
    event.c_str(), command.c_str());

  int r = -1;
  bool found = false;

  for(int i = 0; i < NUM_CLOUD_EVENTS && !found; i++ ){
    String e = CLOUD_EVENTS[i];

    if(event.startsWith(e)){
      r = CLOUD_FUNCS[i](command);
      found = true;
    }
  }

  if(found){
    publish("bpt:event",
      String::format("%u,%s,%i", EVENT_SERIAL_COMMAND, event.c_str(), r));
  }
}

/**
 * Reads serial input and calls the matching cloud function with the passed in
 * command. Format: CALL[<event name>~<command>]\n
 *
 * NB: the function is latched, don't send commands too quickly (wait for OK)
 */
void serialEvent(){
  char c = Serial.read();

  if(serialLatch){
    Serial.println("NOK");
    Serial.println("app: warning - serial buffer not accepting input");
    return;
  }

  serialBuffer[serialBufferIndex++] = c;

  if( c != '\n' && serialBufferIndex >= SERIAL_COMMAND_BUFFER_SIZE ){
    Serial.println("app: warning - serial input exceeds buffer, discarding command");
    serialBufferIndex = 0;
  }

  if(c == '\n'){
    serialLatch = true;
    serialBuffer[serialBufferIndex - 1] = '\0'; // squash newline

    // example CALL[bpt:state~]
    String s = String(serialBuffer).replace("CALL[", "");
    if(s.charAt(s.length() - 1) == ']'){
      s = s.substring(0, s.length() - 1);
    }
    int sep = s.indexOf('~');
    _processSerialCommand(s.substring(0, sep), s.substring(sep + 1));
    Serial.println("OK");
    serialBufferIndex = 0;
    serialLatch = false;
  }
}

//override
void setup() {
  Serial.begin(9600); // opens up a Serial port

  pinMode(ON_BOARD_LED, OUTPUT);
  appCtx.devices = devices;

  controller.setup();

  for(int i = 0; i < NUM_CLOUD_EVENTS; i++ ){
    Particle.function( CLOUD_EVENTS[i], CLOUD_FUNCS[i] );
  }

  /*
  Particle.function("bpt:state", stateFn);
  Particle.function("bpt:gps", gpsCoordFn);
  Particle.function("bpt:status", getStatusFn);
  Particle.function("bpt:diag", getDiagnosticFn);
  Particle.function("bpt:register", registerRemoteDeviceFn);
  Particle.function("bpt:ack", ackEventFn);
  Particle.function("bpt:probe", probeControllerFn);
  Particle.function("bpt:test", testInputFn);
  */

  //FIXME: clear out any received firmware commands (ex COMMAND ATE1 E0)
  // TODO: this doesn't rectify the problem
  serialLatch = false;
  //serialBuffer[0] = '\0';

}
