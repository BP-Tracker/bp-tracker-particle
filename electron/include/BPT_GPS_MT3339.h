#include "BPT_GPS.h"
#include "Adafruit_GPS.h"

#ifndef BPT_GPS_MT3339_h
#define BPT_GPS_MT3339_h

class BPT_GPS_MT3339: public BPT_GPS {

  public:

    BPT_GPS_MT3339(application_ctx_t *appCtx);

    ~BPT_GPS_MT3339();

    void init();

    void init(external_device_t *device);

    bool enable(void);

    bool disable(void);

    bool reset(void);

    // returns true if module has a GPS fix and coords have been updated
    bool getGPSCoord(gps_coord_t *gpsCoord);

  protected:

    static Adafruit_GPS driver;

};

#endif
