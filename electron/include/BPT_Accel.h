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

    virtual void init(void);

    virtual bool enable(void);

    virtual bool disable(void);

    virtual bool reset(void);

    virtual void shutdown(void);

    virtual bool update(void);

    mod_type_t getType(void);

    // returns 1 if acceleration data exists and sets the data in accel,
    // 0 otherwise
		virtual int getAcceleration(accel_t *accel);

    float getMagnitude(accel_t *accel);

  private:

    #ifdef EXTERNAL_DEVICE_LIS3DH
      BPT_Accel_LIS3DH _deviceImpl;
    #else
      BPT_Device_Impl _deviceImpl;
    #endif

};

#endif
