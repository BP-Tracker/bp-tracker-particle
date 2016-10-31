#include "BPT_Module.h"

// Accelerometer include
#ifdef EXTERNAL_DEVICE_LIS3DH
  #include "BPT_Accel_LIS3DH.h"
#else
  #include "BPT_Device_Impl.h"
#endif

#ifndef BPT_Accel_h
#define BPT_Accel_h

class BPT_Accel: public BPT_Module {

  public:

    BPT_Accel(application_ctx_t *appCtx);

    ~BPT_Accel();

    void init(void);

    bool enable(void);

    bool disable(void);

    bool reset(void);

    void shutdown(void);

    bool update(void);

    mod_type_t getType(void);

    bool getStatus(uint16_t mask);

    void setStatus(uint16_t status);

    void setStatus(uint16_t status, const char *msg);

    void clearStatus(uint16_t status);

    char *getStatusMsg();

    void setStatusMsg(const char *msg);

    // returns 1 if acceleration data exists and sets the data in accel,
    // 0 otherwise
		virtual int getAcceleration(accel_t *accel);

    float getMagnitude(accel_t *accel);

    // set wake to true to simulate a movement event
    void setTestData(bool wake, bool reset = false);

  private:

    bool _hasTestData;

    bool _wakeMode;

    #ifdef EXTERNAL_DEVICE_LIS3DH
      BPT_Accel_LIS3DH _deviceImpl;
    #else
      BPT_Device_Impl _deviceImpl;
    #endif

};

#endif
