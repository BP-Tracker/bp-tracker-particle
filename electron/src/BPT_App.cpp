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

/*********************************************
  Functions
*********************************************/

int getState(String command){
  controller_state_t s = controller.getState();
  Serial.printf("getState called: [state=%u]", s);

  Particle.publish("bpt:state", String::format("%u", s), 60, PRIVATE);

  return s;
}

// returns 0 if no GPS data is available, and a number greater
// than zero indicates the number of satelites in the gps signal
int getGpsCoord(String command){
  int r = controller.getGpsCoord(&gpsCoord);
  float lat = r == 0 ? 0 : gpsCoord.lat;
  float lon = r == 0 ? 0 : gpsCoord.lon;

  Serial.printf("getGPSCoord called: [status=%u][lat=%f][log=%f]", r, lat, lon);

  Particle.publish("bpt:gps", String::format("%f,%f", lat, lon), 60, PRIVATE);
  return r;
}


void setup() {
  Serial.begin(9600); // opens up a Serial port

  pinMode(ON_BOARD_LED, OUTPUT);

  appCtx.devices = devices;

  controller.setup();

  // These three functions are useful for remote diagnostics. Read more below.
  Particle.function("bpt:state", getState);
  Particle.function("bpt:gps", getGpsCoord);
}

void loop(){

  controller.loop();

  Serial.println("Setting LED HIGH");
  digitalWrite(ON_BOARD_LED, HIGH);
  delay(5000);

  Serial.println("Setting LED LOW");
  digitalWrite(ON_BOARD_LED, LOW);
  delay(5000);


}
