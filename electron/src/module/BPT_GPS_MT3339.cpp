/* Logic borrowed from particle.io's AssetTracker library
  	(https://github.com/spark/AssetTracker)
*/
#include "BPT_GPS_MT3339.h"

BPT_GPS_MT3339::BPT_GPS_MT3339(application_ctx_t *applicationCtx)
  : BPT_Device_Impl(applicationCtx){ }

BPT_GPS_MT3339::~BPT_GPS_MT3339(){ }

bool BPT_GPS_MT3339::enable(void){

  if(!getStatus(MOD_STATUS_ONLINE)){
    return false;
  }

  if(getStatus(MOD_STATUS_ENABLED)){
    return true;
  }

  driver.begin(9600);
  driver.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  delay(500);
  // Default is 1 Hz update rate
  driver.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  delay(500);
  driver.sendCommand(PGCMD_NOANTENNA);
  delay(500);

  setStatus(MOD_STATUS_ENABLED);
  return true;
}

bool BPT_GPS_MT3339::disable(void){

  // TODO: complete logic

  clearStatus(MOD_STATUS_ENABLED);
  return false;
}

bool BPT_GPS_MT3339::update(void){
  //char c = driver.read();

  driver.read();
    // if a sentence is received, we can check the checksum, parse it...
  if (driver.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(gpss.lastNMEA());   // this also sets the newNMEAreceived() flag to false

    if (!driver.parse(driver.lastNMEA())) {
      // this also sets the newNMEAreceived() flag to false
      //TODO: is this return fine?
      return true;  // we can fail to parse a sentence in which case we should just wait for another
    }
  }

  return true;
}

bool BPT_GPS_MT3339::reset(void){
  return false;
}

/*
void BPT_GPS_MT3339::init(external_device_t *dev){ //TODO
  device = dev;

  if(device->type != DEVICE_TYPE_GPS){
    const char *m = "Device type not supported";
    setStatus(MOD_STATUS_ERROR, m);
    return;
  }
  init();
}
*/

void BPT_GPS_MT3339::init(void){

  // note EXTERNAL_DEVICE_MT3339 will be defined if this class is used
  // see BPT_GPS
  device = &(applicationCtx->devices[EXTERNAL_DEVICE_MT3339]);

  if(device->type != DEVICE_TYPE_GPS){
    const char *m = "Device type not supported";
    setStatus(MOD_STATUS_ERROR, m);
    return;
  }

  /*
  if(device == 0){
    const char *m = "Cannot call init without an external_device_t";
    setStatus(MOD_STATUS_ERROR, m);
    return;
  }
  */

  uint16_t powerPin = device->wiring_pins[0];

  // Power to the GPS is controlled by a FET connected to D6
  pinMode(powerPin,OUTPUT);
  digitalWrite(powerPin, LOW);

  setStatus(MOD_STATUS_ONLINE);
}

void BPT_GPS_MT3339::shutdown(void){
  if(getStatus(MOD_STATUS_ONLINE)){
    uint16_t powerPin = device->wiring_pins[0];
    digitalWrite(powerPin, HIGH);
    clearStatus(MOD_STATUS_ONLINE);
  }
}

// NB: expects a gps_coord_t reference
//TODO:
int BPT_GPS_MT3339::getIntData(void *gpsCoord, int size){

  if(sizeof(gps_coord_t) != size){ // guard
    //TODO: set error condition
    return 0;
  }

  /*
  if(driver.satellites <= 0){ // no GPS fix
    return 0;
  }
  */

  gps_coord_t *gpsCoordRef = static_cast<gps_coord_t*>(gpsCoord);

  memset(gpsCoordRef, 0, sizeof(gps_coord_t)); // clears the data

  gpsCoordRef->lat = driver.latitudeDegrees;
  gpsCoordRef->lon = driver.longitudeDegrees;

  return driver.satellites; //TODO: return the number of fixed satellites
}

Adafruit_GPS BPT_GPS_MT3339::driver = Adafruit_GPS();
