#include "Particle_AssetTracker.h"
#include "Adafruit_GPS.h"
#include "Adafruit_LIS3DH.h"
#include "math.h"

//----------------- Tracker ----------------//

//#define mySerial Serial1
//Adafruit_GPS gps(&mySerial);
Adafruit_GPS gpss = Adafruit_GPS();
Adafruit_LIS3DH accel = Adafruit_LIS3DH(A2);

AssetTracker::AssetTracker(){

}

void AssetTracker::begin(){
    accel.begin(LIS3DH_DEFAULT_ADDRESS);

    // Default to 5kHz low-power sampling
    accel.setDataRate(LIS3DH_DATARATE_LOWPOWER_5KHZ);

    // Default to 4 gravities range
    accel.setRange(LIS3DH_RANGE_4_G);

    // Turn on the GPS module
    // gpsOn();
}

float AssetTracker::readLat(){
    return gpss.latitude;
}

float AssetTracker::readLon(){
    return gpss.longitude;
}

String AssetTracker::readLatLon(){
    String latLon = String::format("%f,%f",gpss.latitudeDegrees,gpss.longitudeDegrees);
    return latLon;
}

void AssetTracker::gpsOn(){
    // Power to the GPS is controlled by a FET connected to D6
    pinMode(D6,OUTPUT);
    digitalWrite(D6,LOW);
    gpss.begin(9600);
    gpss.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    delay(500);
    // Default is 1 Hz update rate
    gpss.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
    delay(500);
    gpss.sendCommand(PGCMD_NOANTENNA);
    delay(500);
}

void AssetTracker::gpsOff(){
    digitalWrite(D6,HIGH);
}

char* AssetTracker::preNMEA(){
    return gpss.lastNMEA();
}

bool AssetTracker::gpsFix(){
    if(gpss.latitude == 0.0){
        return false;
    }
    else {
        return true;
    }
    //return gps.fix;
}

// char AssetTracker::checkGPS(){
//     char c = gps.read();
//     return c;
// }

void AssetTracker::updateGPS(){
    char c = gpss.read();
      // if a sentence is received, we can check the checksum, parse it...
  if (gpss.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(gpss.lastNMEA());   // this also sets the newNMEAreceived() flag to false

    if (!gpss.parse(gpss.lastNMEA()))   {
      // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
    }
  }
}

int AssetTracker::readX(){
    accel.read();
    return accel.x;
}

int AssetTracker::readY(){
    accel.read();
    return accel.y;
}

int AssetTracker::readZ(){
    accel.read();
    return accel.z;
}

int AssetTracker::readXYZmagnitude(){
    accel.read();
    int magnitude = sqrt((accel.x*accel.x)+(accel.y*accel.y)+(accel.z*accel.z));
    return magnitude;
}

bool AssetTracker::setupLowPowerWakeMode(uint8_t movementThreshold) {
	return accel.setupLowPowerWakeMode(movementThreshold);
}

uint8_t AssetTracker::clearAccelInterrupt() {
	return accel.clearInterrupt();
}
