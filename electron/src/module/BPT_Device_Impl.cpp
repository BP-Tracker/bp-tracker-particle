#include "BPT_Device_Impl.h"


BPT_Device_Impl::BPT_Device_Impl(application_ctx_t *applicationCtx)
  : BPT_Module(applicationCtx)
  { }

BPT_Device_Impl::~BPT_Device_Impl(){ }

// default implementation
void BPT_Device_Impl::init(void){
}

void BPT_Device_Impl::shutdown(){
}

bool BPT_Device_Impl::update(void){
  return false;
}

bool BPT_Device_Impl::enable(void){
  return false;
}

bool BPT_Device_Impl::disable(void){
  return false;
}

bool BPT_Device_Impl::reset(void){
  return false;
}

mod_type_t BPT_Device_Impl::getType(void){
  return MODULE_TYPE_OTHER;
}

int BPT_Device_Impl::getIntData(void *ref, int size){
  return 0;
}

float BPT_Device_Impl::getFloatData(void *ref, int size){
  return 0;
}
