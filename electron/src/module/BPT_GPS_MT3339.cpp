#include "BPT_GPS_MT3339.h"

BPT_GPS_MT3339::BPT_GPS_MT3339(application_ctx_t *applicationCtx)
  : BPT_GPS(applicationCtx){ }

BPT_GPS_MT3339::~BPT_GPS_MT3339(){ }

bool BPT_GPS_MT3339::enable(void){
  return false;
}

bool BPT_GPS_MT3339::disable(void){
  return false;
}

bool BPT_GPS_MT3339::reset(void){
  return false;
}

void BPT_GPS_MT3339::init(external_device_t *device){

}

void BPT_GPS_MT3339::init(void){

}

bool BPT_GPS_MT3339::getGPSCoord(gps_coord_t *gpsCoord){
  return false;
}

Adafruit_GPS BPT_GPS_MT3339::driver = Adafruit_GPS();
