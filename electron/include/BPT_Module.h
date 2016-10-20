#include "BPT.h"

#ifndef _BPT_Module_h_
#define _BPT_Module_h_

// TODO: do we need all these?
#define  MOD_STATUS_RESERVED             ((uint16_t)0x0001) //???
#define  MOD_STATUS_ONLINE  					   ((uint16_t)0x0002)
#define  MOD_STATUS_IDLE                 ((uint16_t)0x0004)
#define  MOD_STATUS_ACTIVE               ((uint16_t)0x0008)
#define  MOD_STATUS_ERROR                ((uint16_t)0x0010)
#define  MOD_STATUS_SLEEP                ((uint16_t)0x0040)
#define  MOD_STATUS_AVAILABLE            ((uint16_t)0x0080) //????
#define  MOD_STATUS_ENABLED              ((uint16_t)0x0100)

#define MODULE_STATUS_MSG_MAX_LENGTH 128

#define MODULE_GPS		          1
#define MODULE_ACCELEOMETER     2

typedef struct {
	uint16_t status;
	char message[MODULE_STATUS_MSG_MAX_LENGTH];
} mod_status_t;


class BPT_Module: public BPT {
 	public:

  	BPT_Module(application_ctx_t *applicationCtx);
    virtual ~BPT_Module();

  	virtual void init(void) = 0;

		virtual void init(external_device_t *device) = 0;

		virtual bool enable(void) = 0;

    virtual bool disable(void) = 0;

    virtual bool reset(void) = 0; // TODO: what state should the module be in?

		virtual void shutdown(void) = 0;

    bool getStatus(uint16_t mask);

    void setStatus(uint16_t status);

		void setStatus(uint16_t status, const char *msg);

    void clearStatus(uint16_t status);

		// give the module a chance to maintain state and return an
		// error if something went off.
		virtual bool update();

    char *getStatusMsg();

    void setStatusMsg(const char *msg);

    //application_ctx_t *applicationCtx;

  protected:
    mod_status_t mod_status;

};

#endif
