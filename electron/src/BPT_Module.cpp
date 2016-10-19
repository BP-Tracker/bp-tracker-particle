#include "BPT_Module.h"

BPT_Module::BPT_Module(application_ctx_t *applicationCtx)
  : BPT(applicationCtx){ }

BPT_Module::~BPT_Module() {}

bool BPT_Module::getStatus(uint16_t mask){
  return false;
}

void BPT_Module::setStatus(uint16_t status){
}

void BPT_Module::resetStatus(uint16_t status){

}

char *BPT_Module::getStatusMsg(){
  return nullptr;
}

void BPT_Module::setStatusMsg(char *msg){

}
