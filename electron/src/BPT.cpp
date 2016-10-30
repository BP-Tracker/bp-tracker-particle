#include "BPT.h"
BPT::BPT(application_ctx_t *applicationCtx)
  : applicationCtx(applicationCtx) {  }

BPT::~BPT(){}


bool BPT::registerProperty(application_property_t prop, BPT *owner){ //TODO
  return false;
}

// template<class T> //TODO
// T BPT::getProperty(application_property_t prop, T defaultValue){
//
//   return defaultValue;
// }


BPT_Storage BPT::storage = BPT_Storage();
