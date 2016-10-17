#include "BPT_GPS.h"

BPT_GPS::BPT_GPS(application_ctx_t *a): BPT_Module(a){ }

BPT_GPS::~BPT_GPS(){ }

void BPT_GPS::init(void){}

bool BPT_GPS::enable(void){
  return false;
}

bool BPT_GPS::disable(void){
  return false;
}

bool BPT_GPS::reset(void){
  return false;
}
