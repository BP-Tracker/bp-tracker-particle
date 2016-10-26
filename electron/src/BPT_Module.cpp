#include "BPT_Module.h"

BPT_Module::BPT_Module(application_ctx_t *applicationCtx)
  : BPT(applicationCtx){ }

BPT_Module::~BPT_Module() {}

bool BPT_Module::getStatus(uint16_t mask){
  //FIXME: should we return true if all mask bits are on?
  return (mod_status.status & mask) > 0 ? true : false;
}

void BPT_Module::setStatus(uint16_t status){
  mod_status.status |= status;
  // TODO: auto clear msg?
}

// NB: const char *msg: cannot modify the pointer to msg
// max msg size is MODULE_STATUS_MSG_MAX_LENGTH (128 bytes)
void BPT_Module::setStatus(uint16_t status, const char *msg){
    setStatus(status);
    strcpy(mod_status.message, msg);
}

void BPT_Module::clearStatus(uint16_t status){
  mod_status.status &= ~status;
}

char *BPT_Module::getStatusMsg(){
  return mod_status.message;
}

bool BPT_Module::update(){
  return true; // noop
}

void BPT_Module::setStatusMsg(const char *msg){
  strcpy(mod_status.message, msg);
}
