#include "BPT_Accel.h"
#include <cmath>

BPT_Accel::BPT_Accel(application_ctx_t *applicationCtx)
  : BPT_Module(applicationCtx){ }

BPT_Accel::~BPT_Accel(){ }

// default implementation
void BPT_Accel::init(void){
}

void BPT_Accel::init(external_device_t *device){
}

void BPT_Accel::shutdown(){

}

bool BPT_Accel::update(void){
  return true;
}

bool BPT_Accel::enable(void){
  return false;
}

bool BPT_Accel::disable(void){
  return false;
}

bool BPT_Accel::reset(void){
  return false;
}

mod_type_t BPT_Accel::getType(void){
  mod_type_t t = MODULE_TYPE_ACCELEOMETER;
  return t;
}

float BPT_Accel::getMagnitude(accel_t *accel){
  return sqrt((accel->x * accel->x) + (accel->y * accel->y)
		+ (accel->z * accel->z));
}

int BPT_Accel::getAcceleration(accel_t *accel){
  return -1;
}
