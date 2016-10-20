#include "BPT_GPS.h"

BPT_GPS::BPT_GPS(application_ctx_t *applicationCtx)
  : BPT_Module(applicationCtx){ }

BPT_GPS::~BPT_GPS(){ }

// default implementation
void BPT_GPS::init(void){
}

void BPT_GPS::init(external_device_t *device){
}

void BPT_GPS::shutdown(){

}

bool BPT_GPS::update(void){
  return true;
}

bool BPT_GPS::enable(void){
  return false;
}

bool BPT_GPS::disable(void){
  return false;
}

bool BPT_GPS::reset(void){
  return false;
}

int BPT_GPS::getGpsCoord(gps_coord_t *gpsCoord){
  return 0;
}

float BPT_GPS::getDistanceTo(gps_coord_t *gpsCoord){
  return 0.0;
}

float BPT_GPS::getDistanceTo(gps_coord_t *gpsCoord, distance_calc_t formula){
  return 0.0;
}
