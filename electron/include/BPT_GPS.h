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

// Mean radius of Earth in meters
#define MEAN_EARTH_RADIUS 6371e+3

#define MATH_PI_CONSTANT 3.141592653

class BPT_GPS: public BPT_Module {

  public:

    BPT_GPS(application_ctx_t *appCtx);

    ~BPT_GPS();

    void init(void);

    bool enable(void);

    bool disable(void);

    bool reset(void);

    void shutdown(void);

    bool update(void);

    //BPT_Module overrides
    mod_type_t getType(void);

    bool getStatus(uint16_t mask);

    void setStatus(uint16_t status);

    void setStatus(uint16_t status, const char *msg);

    void clearStatus(uint16_t status);

    char *getStatusMsg();

    void setStatusMsg(const char *msg);


    // returns true ( 0 or greater) if module has a GPS fix and coords have been updated
    // enable lastKnownPosition to get the last known coordinate if no
    // GPS fix is available
    int getGpsCoord(gps_coord_t *gpsCoord, bool useLastKnown = false);

    void setTestData(const gps_coord_t *gpsCoord, bool reset = false,
      int age = 0);

    float getDistanceTo(gps_coord_t *gpsCoord);

    float getDistanceTo(gps_coord_t *gpsCoord, distance_calc_t formula);

  private:

    float _toRadians(float degrees);

    float _toDegrees(float radians);

    // TODO: would it be better if allocation was dynamic?
    #ifdef EXTERNAL_DEVICE_MT3339
      BPT_GPS_MT3339 _deviceImpl;
    #else
      BPT_Device_Impl _deviceImpl;
    #endif

    bool _hasTestData;
    gps_coord_t _testCoord;
    int _testGpsCoordAge;


};

#endif
