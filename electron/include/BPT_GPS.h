#include "BPT_Module.h"

#ifndef BPT_GPS_h
#define BPT_GPS_h

// Default GPS point-to-point distance formula (HAVERSINE_FORMULA)
#define DEFAULT_DISTANCE_CALC ((uint8_t)0x01)

// Geofence radius in meters
#define DEFAULT_GEOFENCE_RADIUS 500

// Mean radius of Earth in meters
#define MEAN_EARTH_RADIUS 6371e+3

#define MATH_PI_CONSTANT 3.141592653

class BPT_GPS: public BPT_Module {

  public:

    BPT_GPS(application_ctx_t *appCtx);

    ~BPT_GPS();

    virtual void init(void);

    virtual void init(external_device_t *device);

    virtual bool enable(void);

    virtual bool disable(void);

    virtual bool reset(void);

    virtual void shutdown(void);

    virtual bool update(void);

    mod_type_t getType(void);

    // returns true if module has a GPS fix and coords have been updated
    virtual int getGpsCoord(gps_coord_t *gpsCoord); // base class return test data

    float getDistanceTo(gps_coord_t *gpsCoord);

    float getDistanceTo(gps_coord_t *gpsCoord, distance_calc_t formula);

  private:

    float toRadians(float degrees);

    float toDegrees(float radians);

};

#endif
