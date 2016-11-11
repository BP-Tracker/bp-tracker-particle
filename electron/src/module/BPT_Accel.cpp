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

// TODO: maybe use GPS intead for this?
int BPT_Accel::isMoving(){

  // TODO: use rolling window

  //FIXME: for now just get a rough idea using the magnitude alone
  const int samples = 10;
  const float base_mag = 0.98;
  const float epsilion = 0.15;

  accel_t a;
  float m;
  for(int i = 0; i < samples; i++){
    getAcceleration(&a);
    m = getMagnitude(&a);

    if(m > base_mag + epsilion || m < base_mag - epsilion ){
      return 1;
    }
  }

  return 0;
}

int BPT_Accel::getAcceleration(accel_t *accel){
  return _deviceImpl.getIntData(accel, sizeof(accel_t));
}

bool BPT_Accel::hasMoved(bool rst){

  if(_hasTestData && applicationCtx->mode == CONTROLLER_MODE_TEST){
    return _wakeMode;
  }

  bool s = _deviceImpl.getStatus( MOD_STATUS_INTERRUPT );

  if(rst && s){
    reset();
  }
  return s;
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
