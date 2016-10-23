/*
  Distance formulas from: http://www.moveable-type.co.uk
  (https://github.com/chrisveness/geodesy)
*/

#include "BPT_GPS.h"
#include <cmath>

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

// uses the default distance formula (DEFAULT_DISTANCE_CALC)
// (also see www.movable-type.co.uk/scripts/latlong.html)
float BPT_GPS::getDistanceTo(gps_coord_t *gpsCoord){
  distance_calc_t c = static_cast<distance_calc_t>(DEFAULT_DISTANCE_CALC);
  return getDistanceTo(gpsCoord, c);
}

float BPT_GPS::getDistanceTo(gps_coord_t *gpsCoord, distance_calc_t formula){
  float d = -1; // default if formula not supported
  gps_coord_t deviceCoord;

  int r = getGpsCoord(&deviceCoord);

  Serial.printf("getDistanceTo called device coord: [lat=%f][lon=%f]\n",
    deviceCoord.lat, deviceCoord.lon);

  if(r <= 0){
    return d; // -1 no gps fix
  }

  // theta = latitude, lambda = longitude
  // source point (point 1)
  float deviceLatR = toRadians(deviceCoord.lat);
  float deviceLonR = toRadians(deviceCoord.lon);

  // destination point (point 2)
  float toLatR = toRadians(gpsCoord->lat);
  float toLonR = toRadians(gpsCoord->lon);

  float deltaLat = toLatR - deviceLatR;
  float deltaLon = toLonR - deviceLonR;

  if(formula == HAVERSINE_FORMULA){

    float a = sin(deltaLat/2) * sin(deltaLat/2)
      + cos(deviceLatR) * cos(toLatR)
      * sin(deltaLon/2) * sin(deltaLon/2);
    float c = 2 * atan2(sqrt(a), sqrt(1-a));
    d = MEAN_EARTH_RADIUS * c;

  }else if(formula == LAW_OF_COSINES_FORMULA){

    d = acos( sin(deviceLatR) * sin(toLatR)
      + cos(deviceLatR) * cos(toLatR) * cos(deltaLon) )
      * MEAN_EARTH_RADIUS;

  }else if(formula == EQUIRECT_APPROXIMATION){
    float x = deltaLon * cos((deviceLatR + toLatR)/2);
    d = sqrt(x * x + deltaLat * deltaLat) * MEAN_EARTH_RADIUS;
  }

  return d;
}

float BPT_GPS::toRadians(float degrees){
  return (degrees * MATH_PI_CONSTANT) / 180;
}

float BPT_GPS::toDegrees(float radians){
  return (radians * 180) / MATH_PI_CONSTANT;
}
