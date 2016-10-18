//#include "Particle_AssetTracker.h"
#include "BPT_Controller.h"
#include "BPT_GPS.h"

// constants
int ON_BOARD_LED = D7;


application_ctx_t appCtx;
BPT_Controller controller = BPT_Controller(&appCtx);
// //BPT_Accelerometer accel = new BPT_Accelerometer(sys);
// //BPT_PowerMonitor pmon = new BPT_PowerMonitor(sys);
//
// system_state_t state;

int getState(String command){
  system_state_t s = controller.getState();
  Serial.printf("getState called: [state=%u]", s);

  Particle.publish("bpt:state", String::format("%u", s), 60, PRIVATE);

  return s;
}


void setup() {
  Serial.begin(9600); // opens up a Serial port

  pinMode(ON_BOARD_LED, OUTPUT);

  controller.setup();

  //TODO: detect interested modules attached to the hardware

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
