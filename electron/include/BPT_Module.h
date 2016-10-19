/**************************************************************************/
/*!
    @file     BPT_Module.h
    @author   Derek Benda
    @license  LPGL (see Licence.txt)

    v1.0  - First release
*/
/**************************************************************************/
#include "BPT.h"

#ifndef _BPT_Module_h_
#define _BPT_Module_h_

// TODO: do we need all these?
#define  MOD_STATUS_ONLINE               ((uint16_t)0x0001)
#define  MOD_STATUS_OFFLINE 					   ((uint16_t)0x0002)
#define  MOD_STATUS_IDLE                 ((uint16_t)0x0004)
#define  MOD_STATUS_ACTIVE               ((uint16_t)0x0008)
#define  MOD_STATUS_ERROR                ((uint16_t)0x0010)
#define  MOD_STATUS_SLEEP                ((uint16_t)0x0040)
#define  MOD_STATUS_AVAILABLE            ((uint16_t)0x0080)
#define  MOD_STATUS_ENABLED              ((uint16_t)0x0100)
#define  MOD_STATUS_DISABLED             ((uint16_t)0x0200)

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

    virtual bool reset(void) = 0;

    bool getStatus(uint16_t mask);

    void setStatus(uint16_t status);

    void resetStatus(uint16_t status);

    char *getStatusMsg();

    void setStatusMsg(char *msg);

    //application_ctx_t *applicationCtx;

  protected:
    mod_status_t mod_status;

};

#endif
