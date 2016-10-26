//#include "Particle_AssetTracker.h"
#include "BPT.h"
#include "BPT_GPS.h"
#include "BPT_Controller.h"

/*********************************************
  Constants
*********************************************/
int ON_BOARD_LED = D7;

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
accel_t accelData;
unsigned long stateTime = 0;
BPT_Controller controller = BPT_Controller(&appCtx);
FuelGauge fuelGauge;

/*********************************************
  Functions
*********************************************/

// returns the current state of the controller (controller_state_t type)
int getState(String command){
  controller_state_t s = controller.getState();
  Serial.printf("getState called: [state=%u]", s);

  Particle.publish("bpt:state", String::format("%u", s), 60, PRIVATE);

  return s;
}

// returns the current state of the controller including
// gps location and battery status
// format: "controller_mode_t,controller_state_t,batt(%),satellite,lat,lon"
// TODO: thread safe?
int getStatus(String command){
  controller_mode_t m = controller.getMode();
  controller_state_t s = controller.getState();
  int r = controller.getGpsCoord(&gpsCoord);

  Particle.publish("bpt:status",
    String::format("%u,%u,%.2f,%i,%f,%f", m, s,
      fuelGauge.getSoC(), r, gpsCoord.lat, gpsCoord.lon),
    60, PRIVATE);

    return 0;
}

// pass the coordinate to the controller for processing
int _processRemoteGpsCoord(float lat, float lon, String deviceNum){
  gps_coord_t coord;
  coord.lat = lat;
  coord.lon = lon;

  uint8_t devNum = deviceNum.length() > 0 ? atoi(deviceNum) : 1;

  Serial.printf("processRemoteGpsCoord called: [lat=%f][lon=%f][devNum=%u]\n",
    coord.lat, coord.lon, devNum);

  //float d = controller.gpsModule.getDistanceTo(
  //  &coord, LAW_OF_COSINES_FORMULA);

  bool r = controller.receive(&coord, devNum);
  return r == true ? 1 : 0;
}

/*
  This function has a dual purpose:
  1 - when GPS coords are passed in, the controller will use it to
  determine the proximity of the coordinate and update its
  state accordingly. This is used when the controller publishes a request
  to remote devices.

  2 - otherwise, it returns the number of satelites in the gps signal
  and publishes the coords of the device in the format:
  latitude,longitude

  curl https://api.particle.io/v1/devices/Lippy/bpt:gps
   -d access_token=${particle token list }

  TODO: is this thread safe?
*/
// accepts GPS coordinates in the format: "[deviceId:]latitude,lonitude"
int getGpsCoord(String command){
  String deviceId = "";

  int sep = command.lastIndexOf(",");
  int deviceSep = command.lastIndexOf(":");

  if( deviceSep > 0){ // found device number (id)
    deviceId = command.substring(0, deviceSep);
    deviceSep++;
  }else{
    deviceSep = 0;
  }

  if(sep > 0){ // found GPS coords
    String latS = command.substring(deviceSep, sep);
    String lonS = command.substring(sep + 1);
    return _processRemoteGpsCoord(atof(latS), atof(lonS), deviceId);
  }

  int r = controller.getGpsCoord(&gpsCoord);
  float lat = r == 0 ? 0 : gpsCoord.lat;
  float lon = r == 0 ? 0 : gpsCoord.lon;

  Serial.printf("getGPSCoord called: [status=%u][lat=%f][log=%f]",
    r, lat, lon);

  if(r > 0){
    Particle.publish("bpt:gps", String::format("%f,%f", lat, lon),
      60, PRIVATE);
  }else{
     //TODO: perhaps send something else ?
    Particle.publish("bpt:gps", "0,0", 60, PRIVATE);
  }
  return r;
}

// publishes more information about the state of controller for troubleshooting
// or diagnostic purposes
// command : the level of output
int getDiagnostic(String command){
  // TODO
  /*
  uint32_t freeMem = System.freeMemory();
  int firmwareVers = System.versionNumber();
  unsigned long runTime = millis();
  size_t eepromLen = EEPROM.length();
  */
  uint32_t freeMem = System.freeMemory();
  uint16_t status = controller.accelModule.mod_status.status;
  bool s = controller.accelModule.getStatus(MOD_STATUS_INTERRUPT);
  controller.accelModule.getAcceleration(&accelData);
  int mag = controller.accelModule.getMagnitude(&accelData);

  Serial.printf("getDiagnostic called: [status=%u][x=%f][y=%f][z=%f][m=%i][mem=%i][it=%i]\n",
    status, accelData.x, accelData.y, accelData.z, mag, freeMem, s == true ? 1 : 0);

  /*
  Particle.publish("bpt:diag",
    String::format("%u,%f,%f,%f, %i", status, a.x, a.y, a.z, mag),
    60, PRIVATE);
    */
  return 1;
}

/*
  Registers a remote device as a candidate to respond to this controller.
  This is optional when only one device will be used.
*/
int registerRemoteDevice(String command){
  //TODO
  return -1;
}


void setup() {
  Serial.begin(9600); // opens up a Serial port

  pinMode(ON_BOARD_LED, OUTPUT);

  appCtx.devices = devices;

  //delay(10000);

  controller.setup();

  Particle.function("bpt:state", getState);
  Particle.function("bpt:gps", getGpsCoord);
  Particle.function("bpt:status", getStatus);
  Particle.function("bpt:diag", getDiagnostic);
  Particle.function("bpt:register", registerRemoteDevice);
}

void loop(){
  controller.loop();
  //bool gpsOnline = controller.gpsModule.getStatus(MOD_STATUS_ONLINE);

  if (millis() - stateTime > 10000) {
    stateTime = millis();

    if( digitalRead(WKP) == HIGH){
      Serial.println("loop[wkp=HIGH]");
    }else{
      Serial.println("loop[wkp=LOW]");
    }

  }
  /*
  int ver = appCtx.devices[0].version;
  uint16_t status = controller.gpsModule.mod_status.status;
  char *message = controller.gpsModule.mod_status.message;


  Serial.printf("GPS module [version=%i][status=%u][message=%s]\n",
    ver, status, message);

  delay(5000);
  */

  /*
  Serial.println("Setting LED HIGH");
  digitalWrite(ON_BOARD_LED, HIGH);
  delay(5000);

  Serial.println("Setting LED LOW");
  digitalWrite(ON_BOARD_LED, LOW);
  delay(5000);
  */


}
