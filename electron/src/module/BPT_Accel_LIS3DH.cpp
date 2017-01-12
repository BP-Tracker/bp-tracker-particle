/**
 * Logic borrowed from particle.io's AssetTracker library
 * See https://github.com/spark/AssetTracker
 */
#include "BPT_Accel_LIS3DH.h"

BPT_Accel_LIS3DH::BPT_Accel_LIS3DH(application_ctx_t *applicationCtx)
  : BPT_Device_Impl(applicationCtx){ }

BPT_Accel_LIS3DH::~BPT_Accel_LIS3DH(){ }

// Override
bool BPT_Accel_LIS3DH::getStatus(uint16_t mask){
  // Serial.printf("[LIS3DH_REG_INT1SRC=%u]\n", intSrc);

  // check if wakeup interrupt was generated
  // TODO: revise the driver API
  uint8_t intSrc = driver.clearInterrupt(false);
  if( (intSrc & LIS3DH_INT1_SRC_IA) != 0 ){
      setStatus(MOD_STATUS_INTERRUPT);
  }

  return BPT_Device_Impl::getStatus(mask);
}

bool BPT_Accel_LIS3DH::enable(void){ //TODO

  if(!getStatus(MOD_STATUS_ONLINE)){
    return false;
  }

  if(getStatus(MOD_STATUS_ENABLED)){
    return true;
  }

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
/* Resets any interrupts (if it was triggered) */
// TODO: also re-register property?
bool BPT_Accel_LIS3DH::reset(void){   //TODO
  if(getStatus(MOD_STATUS_INTERRUPT)){
    driver.clearInterrupt(true);
    clearStatus(MOD_STATUS_INTERRUPT);
  }

  return true;
}

void BPT_Accel_LIS3DH::init(void){

  device = &(applicationCtx->devices[EXTERNAL_DEVICE_LIS3DH]);

  if(device->type != DEVICE_TYPE_ACCEL){
    const char *m = "Device type not supported";
    setStatus(MOD_STATUS_ERROR, m);
    return;
  }

  uint8_t threshold = DEFAULT_PROP_ACCEL_THRESHOLD;
  registerProperty(PROP_ACCEL_THRESHOLD, threshold);

  bool success = getProperty(PROP_ACCEL_THRESHOLD, threshold);
  if(!success){
    const char *m = "cannot get accel_threshold property using default";
    setStatus(MOD_STATUS_ERROR, m);
  }

  driver.begin(LIS3DH_DEFAULT_ADDRESS);

  // Default to 5kHz low-power sampling
  driver.setDataRate(LIS3DH_DATARATE_LOWPOWER_5KHZ);

  // Default to 4 gravities range
  driver.setRange(LIS3DH_RANGE_4_G);


  // Setup wakeup interrupt
  driver.setupLowPowerWakeMode(threshold);

  setStatus(MOD_STATUS_ONLINE);
}

//override
bool BPT_Accel_LIS3DH::updateLocalProperty(BPT_Storage* storage,
    application_property_t prop, String value, bool persistent){

  bool success = true;

  if( prop != PROP_ACCEL_THRESHOLD ){
    return BPT_Device_Impl::updateLocalProperty(storage, prop, value);
  }

  int temp = value.toInt();
  if(temp < 0 || temp > 16){ // per driver specs
    return false;
  }

  uint8_t thres = (uint8_t)temp;

  driver.setupLowPowerWakeMode(thres);
  if(persistent){
    success = storage->setProperty(prop, thres);
  }
  return success;
}

void BPT_Accel_LIS3DH::shutdown(void){ //TODO
   if(getStatus(MOD_STATUS_ONLINE)){

     clearStatus(MOD_STATUS_ONLINE);
   }
}

// returns proper acceleration of the device
// accel arg expects a reference to accel_t
int BPT_Accel_LIS3DH::getIntData(void *accel, int size){

  if(sizeof(accel_t) != size){ // guard
    const char *m = "getIntData expects accel_t ref";
    setStatus(MOD_STATUS_ERROR, m);
    return 0;
  }

  accel_t *accelRef = static_cast<accel_t*>(accel);

  driver.read();
  memset(accelRef, 0, sizeof(accel_t)); // clears the data

  accelRef->x = driver.x_g;
  accelRef->y = driver.y_g;
  accelRef->z = driver.z_g;

  return 1;
}


// FIXME: find a way to configure the driver pins using external_device_type_t
Adafruit_LIS3DH BPT_Accel_LIS3DH::driver = Adafruit_LIS3DH( A2 );
