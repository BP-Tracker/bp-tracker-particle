/* Logic borrowed from particle.io's AssetTracker library
  	(https://github.com/spark/AssetTracker)
*/
#include "BPT_Accel_LIS3DH.h"

BPT_Accel_LIS3DH::BPT_Accel_LIS3DH(application_ctx_t *applicationCtx)
  : BPT_Accel(applicationCtx){ }

BPT_Accel_LIS3DH::~BPT_Accel_LIS3DH(){ }

bool BPT_Accel_LIS3DH::enable(void){

  if(!getStatus(MOD_STATUS_ONLINE)){
    return false;
  }

  if(getStatus(MOD_STATUS_ENABLED)){
    return true;
  }

  // TODO
  return true;
}

bool BPT_Accel_LIS3DH::disable(void){

  // TODO: complete logic

  clearStatus(MOD_STATUS_ENABLED);
  return false;
}

bool BPT_Accel_LIS3DH::update(void){

  return true;
}

bool BPT_Accel_LIS3DH::reset(void){
  return false;
}

void BPT_Accel_LIS3DH::init(external_device_t *dev){ //TODO
  device = dev;

  if(device->type != DEVICE_TYPE_ACCEL){
    const char *m = "Device type not supported";
    setStatus(MOD_STATUS_ERROR, m);
    return;
  }
  init();
}

void BPT_Accel_LIS3DH::init(void){

  if(device == 0){
    const char *m = "Cannot call init without an external_device_t";
    setStatus(MOD_STATUS_ERROR, m);
    return;
  }
	//TODO
  // uint16_t powerPin = device->wiring_pins[0];
	//
  // // Power to the GPS is controlled by a FET connected to D6
  // pinMode(powerPin,OUTPUT);
  // digitalWrite(powerPin, LOW);

  setStatus(MOD_STATUS_ONLINE);
}

void BPT_Accel_LIS3DH::shutdown(void){
  // if(getStatus(MOD_STATUS_ONLINE)){
  //   uint16_t powerPin = device->wiring_pins[0];
  //   digitalWrite(powerPin, HIGH);
  //   clearStatus(MOD_STATUS_ONLINE);
  // }
}

// FIXME: find a way to configure the driver pins using external_device_type_t
Adafruit_LIS3DH BPT_Accel_LIS3DH::driver = Adafruit_LIS3DH( A2 );
