#include "BPT_Device_Impl.h"
#include "Adafruit_GPS.h"

#ifndef BPT_GPS_MT3339_h
#define BPT_GPS_MT3339_h

class BPT_GPS_MT3339: public BPT_Device_Impl {

  public:

    BPT_GPS_MT3339(application_ctx_t *appCtx);

    ~BPT_GPS_MT3339();

    void init();

    bool enable(void);

    bool disable(void);

    bool reset(void);

    void shutdown(void);

    bool update(void);

    // returns true if module has a GPS fix and coords have been updated
    int getIntData(void *gpsCoord, int size);

  protected:

    static Adafruit_GPS driver;

  private:

    // if this is enabled, then the module will not turn on the GPS
    bool simulationMode;
    external_device_t *device;

};

#endif
