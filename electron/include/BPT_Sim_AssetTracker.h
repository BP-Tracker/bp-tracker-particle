/* Simulates GPS and accelerometer activity */

#include "Particle_AssetTracker.h"

#ifndef BPT_Sim_AssetTracker_h
#define BPT_Sim_AssetTracker_h


class BPT_Sim_AssetTracker: public Particle_AssetTracker {

 public:

  BPT_Sim_AssetTracker();

  void
    begin(void),
    updateGPS(void),
    gpsOn(void),
    gpsOff(void);
  int
    readX(void),
    readY(void),
    readZ(void),
    readXYZmagnitude(void);
  float
    readLat(void),
    readLon(void);
  bool
    gpsFix(void);
  char
    checkGPS(void),
    *preNMEA(void);
  String
    readLatLon(void);

  bool
    setupLowPowerWakeMode(uint8_t movementThreshold = 16);
  uint8_t
    clearAccelInterrupt();


 private:

};

#endif
