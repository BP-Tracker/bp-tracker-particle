/*
  Formulas from: http://www.moveable-type.co.uk
    (https://github.com/chrisveness/geodesy)
*/

#include "BPT_GPS.h"
#include "math.h"

BPT_GPS::BPT_GPS(application_ctx_t *applicationCtx)
  : BPT_Module(applicationCtx),
  #ifdef EXTERNAL_DEVICE_MT3339
    _deviceImpl(BPT_GPS_MT3339(applicationCtx)),
  #else
    _deviceImpl(BPT_Device_Impl(applicationCtx)),
  #endif
  _hasTestData(0),
  _testCoord(gps_coord_t()),
  _testGpsCoordAge(0)
  { }

BPT_GPS::~BPT_GPS(){ }

// default implementation
void BPT_GPS::init(void){
    _deviceImpl.init();
}

void BPT_GPS::shutdown(){
  _deviceImpl.shutdown();
}

bool BPT_GPS::update(void){
  return _deviceImpl.update();
}

bool BPT_GPS::enable(void){
  return _deviceImpl.enable();
}

bool BPT_GPS::disable(void){
  return _deviceImpl.disable();
}

bool BPT_GPS::reset(void){
  return _deviceImpl.reset();
}

mod_type_t BPT_GPS::getType(void){
  return MODULE_TYPE_GPS;
}

bool BPT_GPS::getStatus(uint16_t mask){
  return _deviceImpl.getStatus(mask);
}

void BPT_GPS::setStatus(uint16_t status){
  _deviceImpl.setStatus(status);
}

void BPT_GPS::setStatus(uint16_t status, const char *msg){
  _deviceImpl.setStatus(status, msg);
}

void BPT_GPS::clearStatus(uint16_t status){
  _deviceImpl.clearStatus(status);
}

char *BPT_GPS::getStatusMsg(){
  return _deviceImpl.getStatusMsg();
}

void BPT_GPS::setStatusMsg(const char *msg){
  return _deviceImpl.setStatusMsg(msg);
}

int BPT_GPS::getGpsCoord(gps_coord_t *gpsCoord, bool useLastKnown){
  //TODO: return useLastKnown if data not available

  if(_hasTestData && applicationCtx->mode == CONTROLLER_MODE_TEST){

    memset(gpsCoord, 0, sizeof(gps_coord_t));
    gpsCoord->lat = _testCoord.lat;
    gpsCoord->lon = _testCoord.lon;

    return _testGpsCoordAge;
  }

  return _deviceImpl.getIntData(gpsCoord, sizeof(gps_coord_t));
}

// sets this coordinate info to be returned by getGpsCoord when
// the controller mode is CONTROLLER_MODE_TEST
void BPT_GPS::setTestData(const gps_coord_t *gpsCoord, bool reset, int age){
  _hasTestData = !reset;
  _testGpsCoordAge = age; // negative means no vaild data
  _testCoord.lat = gpsCoord->lat;
  _testCoord.lon = gpsCoord->lon;
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

  if(r < 0){
    return d; // -1 no gps fix
  }

  // theta = latitude, lambda = longitude
  // source point (point 1)
  float deviceLatR = _toRadians(deviceCoord.lat);
  float deviceLonR = _toRadians(deviceCoord.lon);

  // destination point (point 2)
  float toLatR = _toRadians(gpsCoord->lat);
  float toLonR = _toRadians(gpsCoord->lon);

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

float BPT_GPS::_toRadians(float degrees){
  return (degrees * MATH_PI_CONSTANT) / 180;
}

float BPT_GPS::_toDegrees(float radians){
  return (radians * 180) / MATH_PI_CONSTANT;
}
