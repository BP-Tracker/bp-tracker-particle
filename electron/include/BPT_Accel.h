#include "BPT_Module.h"

#ifndef BPT_Accel_h
#define BPT_Accel_h

class BPT_Accel: public BPT_Module {

  public:

    BPT_Accel(application_ctx_t *appCtx);

    ~BPT_Accel();

    virtual void init(void);

    virtual void init(external_device_t *device);

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

};

#endif
