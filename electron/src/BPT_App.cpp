//#include "Particle_AssetTracker.h"
#include "BPT.h"
#include "BPT_GPS.h"
#include "BPT_Controller.h"

/*********************************************
  Constants
*********************************************/
int ON_BOARD_LED = D7;


/*********************************************
  Application variables
*********************************************/
extern external_device_t devices[EXTERNAL_DEVICE_COUNT];
application_ctx_t appCtx;
gps_coord_t gpsCoord;
BPT_Controller controller = BPT_Controller(&appCtx);
FuelGauge fuelGauge;

/*********************************************
  Functions
*********************************************/

// returns the current state of the controller (controller_state_t type)
int getState(String command){

  int seperatorIndex = command.lastIndexOf(",");
  if(seperatorIndex > 0){
    String latS = command.substring(0, seperatorIndex);
    String lonS = command.substring(seperatorIndex + 1);


    gps_coord_t coord;
    coord.lat = atof(latS);
    coord.lon = atof(lonS);

    Serial.printf("getState called: [lat=%f][lon=%f]\n", coord.lat, coord.lon);

    float d = controller.gpsModule.getDistanceTo(
      &coord, LAW_OF_COSINES_FORMULA);

    Particle.publish("bpt:state", String::format("%f", d), 60, PRIVATE);
    return 1;
  }
  return 0;


  /*
  controller_state_t s = controller.getState();
  Serial.printf("getState called: [state=%u]", s);

  Particle.publish("bpt:state", String::format("%u", s), 60, PRIVATE);

  return s;
  */
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

/*
  returns the number of satelites in the gps signal
  and publishes the coordinates

  curl https://api.particle.io/v1/devices/Lippy/bpt:gps
   -d access_token=${particle token list }
*/
// TODO: thread safe?
int getGpsCoord(String command){
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


void setup() {
  Serial.begin(9600); // opens up a Serial port

  pinMode(ON_BOARD_LED, OUTPUT);

  appCtx.devices = devices;

  //delay(10000);

  controller.setup();

  Particle.function("bpt:state", getState);
  Particle.function("bpt:gps", getGpsCoord);
  Particle.function("bpt:status", getStatus);
}

void loop(){
  controller.loop();
  //bool gpsOnline = controller.gpsModule.getStatus(MOD_STATUS_ONLINE);

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
