#include "BPT_Module.h"

#ifndef BPT_Device_Impl_h
#define BPT_Device_Impl_h


class BPT_Device_Impl: public BPT_Module {

  public:

    BPT_Device_Impl(application_ctx_t *appCtx);

    ~BPT_Device_Impl();

    virtual void init(void);

    virtual bool enable(void);

    virtual bool disable(void);

    virtual bool reset(void);

    virtual void shutdown(void);

    virtual bool update(void);

    virtual mod_type_t getType(void);

    // the ref points to an object of size specified in the argument
    virtual int getIntData(void *ref, int size);

    virtual float getFloatData(void *ref, int size);

  private:

};

#endif
