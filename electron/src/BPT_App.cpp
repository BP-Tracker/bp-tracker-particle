//#include "Particle_AssetTracker.h"
#include "BPT_Controller.h"
#include "BPT_GPS.h"


//BPT_System sys = BPT_System();
//BPT_GPS gpss = BPT_GPS(sys);

application_ctx_t app_ctx;


BPT_Controller controller = BPT_Controller(&app_ctx);
// //BPT_Accelerometer accel = new BPT_Accelerometer(sys);
// //BPT_PowerMonitor pmon = new BPT_PowerMonitor(sys);
//
// system_state_t state;


void setup() {

  controller.setup();

  // state = STATE_INIT;
  //
  // sys.init();
  // gps.init();
  // //accel.init();
  // //pmon.init();
  //
  // bool enableSuccess = gps.enable();
  //
  // if(!enableSuccess){
  //   // TODO: .....
  //
  // }

}

void loop(){

  controller.loop();

  // subscribe tp


}
