#include "BPT_Accel.h"
#include "Adafruit_LIS3DH.h"

#ifndef BPT_Accel_LIS3DH_h
#define BPT_Accel_LIS3DH_h

class BPT_Accel_LIS3DH: public BPT_Accel {

  public:

    BPT_Accel_LIS3DH(application_ctx_t *appCtx);

    ~BPT_Accel_LIS3DH();

    void init();

    void init(external_device_t *dev);

    bool enable(void);

    bool disable(void);

    bool reset(void);

    void shutdown(void);

    bool update(void);

    // returns true if module has a GPS fix and coords have been updated
    int getGpsCoord(gps_coord_t *gpsCoord);

  protected:

    static Adafruit_LIS3DH driver;

  private:

    // if this is enabled, then the module will not turn on the GPS
    bool simulationMode;
    external_device_t *device;
};

#endif
