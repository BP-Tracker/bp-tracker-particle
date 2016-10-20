#include "BPT.h"
BPT::BPT(application_ctx_t *applicationCtx)
  : BPT(applicationCtx) {  }

BPT::~BPT(){}


bool BPT::registerProperty(application_property_t prop, BPT *owner){
  return false; //TODO
}

// template<class T>
// T BPT::getProperty(application_property_t prop, T defaultValue){
//   //TODO
//   return defaultValue;
// }


BPT_Storage BPT::storage = BPT_Storage();
