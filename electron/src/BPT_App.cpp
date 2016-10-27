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
unsigned long stateTime = 0;
BPT_Controller controller = BPT_Controller(&appCtx);
FuelGauge fuelGauge;

/*********************************************
  Functions
*********************************************/

// get or set the current state of the controller (controller_state_t type)
// If setting the state, the return is 0 if the state was changed, -1 otherwise
// format [deviceNum:]controller_state_t
int stateFn(String command){
  controller_state_t s = controller.getState();
  Serial.printf("stateFn called: [state=%u]", s);

  Particle.publish("bpt:state", String::format("%u", s), 60, PRIVATE);

  return s;
}

// returns the current state of the controller including
// gps location and battery status
// format: "controller_mode_t,controller_state_t,batt(%),satellite,lat,lon"
// TODO: thread safe?
int getStatusFn(String command){
  controller_mode_t m = controller.getMode();
  controller_state_t s = controller.getState();
  int r = controller.getGpsCoord(&gpsCoord);

  Particle.publish("bpt:status",
    String::format("%u,%u,%.2f,%i,%f,%f", m, s,
      fuelGauge.getSoC(), r, gpsCoord.lat, gpsCoord.lon),
    60, PRIVATE);

    return 0;
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

// pass the coordinate to the controller for processing
int _processRemoteGpsCoord(float lat, float lon, int devNum){
  gps_coord_t coord;
  coord.lat = lat;
  coord.lon = lon;

  Serial.printf("processRemoteGpsCoord called: [lat=%f][lon=%f][devNum=%u]\n",
    coord.lat, coord.lon, devNum);

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
// accepts GPS coordinates in the format: "[deviceNum:]latitude,lonitude"
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
  float lat = r == 0 ? 0 : gpsCoord.lat;
  float lon = r == 0 ? 0 : gpsCoord.lon;

  Serial.printf("gpsCoordFn called: [status=%u][lat=%f][log=%f]",
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
// command : number -> publish the data to the cloud
int getDiagnosticFn(String command){

	//TODO: input validation
	int publishToCloud = atoi(command);

  // TODO
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

	Serial.print("getDiagnosticFn called: ");
  Serial.printf(
		"[status=%u][x=%f][y=%f][z=%f][m=%i][mem=%i][it=%i]\n",
    status, a.x, a.y, a.z, mag, freeMem, s == true ? 1 : 0);

	if(publishToCloud){
	  Particle.publish("bpt:diag",
	    String::format("%u,%f,%f,%f, %i", status, a.x, a.y, a.z, mag),
	    60, PRIVATE);
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

	Serial.printf("ackEventFn called: [devNum=%u][event=%u][data=%s]",
	 	deviceNum, event, data);

	bool r = controller.receive(event, data, deviceNum);

	return r == true ? 1 : -1;
}


void setup() {
  Serial.begin(9600); // opens up a Serial port

  pinMode(ON_BOARD_LED, OUTPUT);

  appCtx.devices = devices;

  controller.setup();

  Particle.function("bpt:state", stateFn);
  Particle.function("bpt:gps", gpsCoordFn);
  Particle.function("bpt:status", getStatusFn);
  Particle.function("bpt:diag", getDiagnosticFn);
  Particle.function("bpt:register", registerRemoteDeviceFn);
	Particle.function("bpt:ack", ackEventFn);
}

void loop(){

	// NB: the controller publishes 'bpt:event' events to the cloud
  controller.loop();

  if (millis() - stateTime > 10000) {
    stateTime = millis();

    if( digitalRead(WKP) == HIGH){
      Serial.println("loop[wkp=HIGH]");
    }else{
      Serial.println("loop[wkp=LOW]");
    }

  }

}
