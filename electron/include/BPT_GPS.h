#include "BPT_Module.h"

#ifndef BPT_GPS_h
#define BPT_GPS_h


class BPT_GPS: public BPT_Module {

  public:

    BPT_GPS(application_ctx_t *a);

    ~BPT_GPS();

    void init(void);

    bool enable(void);

    bool disable(void);

    bool reset(void);



};

#endif
