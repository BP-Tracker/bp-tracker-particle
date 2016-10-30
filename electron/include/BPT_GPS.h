#include "BPT_Module.h"

// GPS module include
#ifdef EXTERNAL_DEVICE_MT3339
  #include "BPT_GPS_MT3339.h"
#else
  #include "BPT_Device_Impl.h"
#endif

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

    // TODO: would it be better if allocation was dynamic?
    #ifdef EXTERNAL_DEVICE_MT3339
      BPT_GPS_MT3339 _deviceImpl;
    #else
      BPT_Device_Impl _deviceImpl;
    #endif

};

#endif
