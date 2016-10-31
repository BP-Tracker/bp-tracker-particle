#include "BPT_Accel.h"
#include "math.h"

BPT_Accel::BPT_Accel(application_ctx_t *applicationCtx)
  : BPT_Module(applicationCtx),
    _hasTestData(false),
    _wakeMode(false),
  #ifdef EXTERNAL_DEVICE_LIS3DH
    _deviceImpl(BPT_Accel_LIS3DH(applicationCtx))
  #else
    _deviceImpl(BPT_Device_Impl(applicationCtx))
  #endif
  { }

BPT_Accel::~BPT_Accel(){ }


void BPT_Accel::init(void){
    _deviceImpl.init();
}

void BPT_Accel::shutdown(){
  _deviceImpl.shutdown();
}

bool BPT_Accel::update(void){
  return _deviceImpl.update();
}

bool BPT_Accel::enable(void){
  return _deviceImpl.enable();
}

bool BPT_Accel::disable(void){
  return _deviceImpl.disable();
}

bool BPT_Accel::reset(void){
  return _deviceImpl.reset();
}

float BPT_Accel::getMagnitude(accel_t *accel){
  return sqrt((accel->x * accel->x) + (accel->y * accel->y)
    + (accel->z * accel->z));
}

int BPT_Accel::getAcceleration(accel_t *accel){
  return _deviceImpl.getIntData(accel, sizeof(accel_t));
}

mod_type_t BPT_Accel::getType(void){
  return MODULE_TYPE_ACCELEOMETER;
}

// override
bool BPT_Accel::getStatus(uint16_t mask){
  if( _hasTestData &&
      applicationCtx->mode == CONTROLLER_MODE_TEST &&
      ( (mask & MOD_STATUS_INTERRUPT) == MOD_STATUS_INTERRUPT ) ){

    return _wakeMode;
  }

  return _deviceImpl.getStatus(mask);
}

void BPT_Accel::setStatus(uint16_t status, const char *msg){
  _deviceImpl.setStatus(status, msg);
}

void BPT_Accel::clearStatus(uint16_t status){
  _deviceImpl.clearStatus(status);
}

char *BPT_Accel::getStatusMsg(){
  return _deviceImpl.getStatusMsg();
}

void BPT_Accel::setStatusMsg(const char *msg){
  return _deviceImpl.setStatusMsg(msg);
}

void BPT_Accel::setTestData(bool wake, bool reset){
  _hasTestData = !reset;
  _wakeMode = wake;
}
