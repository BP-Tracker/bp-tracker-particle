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
BPT_Controller controller = BPT_Controller(&appCtx);

/*********************************************
  Functions
*********************************************/

int getState(String command){
  system_state_t s = controller.getState();
  Serial.printf("getState called: [state=%u]", s);

  Particle.publish("bpt:state", String::format("%u", s), 60, PRIVATE);

  return s;
}


void setup() {
  Serial.begin(9600); // opens up a Serial port

  pinMode(ON_BOARD_LED, OUTPUT);

  appCtx.devices = devices;

  controller.setup();

  // These three functions are useful for remote diagnostics. Read more below.
  Particle.function("bpt:state", getState);
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
