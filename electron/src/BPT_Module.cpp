#include "BPT_Module.h"

BPT_Module::BPT_Module(application_ctx_t *applicationCtx)
  : BPT(applicationCtx){ }

BPT_Module::~BPT_Module() {}

bool BPT_Module::getStatus(uint16_t mask){
  return false;
}

void BPT_Module::setStatus(uint16_t status){

  // TODO: auto clear msg?
}

// NB: const char *msg: cannot modify the pointer to msg
void BPT_Module::setStatus(uint16_t status, const char *msg){

}

void BPT_Module::clearStatus(uint16_t status){

}

char *BPT_Module::getStatusMsg(){
  return nullptr;
}

bool BPT_Module::update(){
  return true; // noop
}

void BPT_Module::setStatusMsg(const char *msg){

}
