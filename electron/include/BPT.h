#include "application.h"

#ifndef _BPT_h_
#define _BPT_h_

typedef enum {
	STATE_UNKNOWN   	  = ((uint8_t)0x00),
	STATE_ARMED					= ((uint8_t)0x01),
	STATE_DISARMED			= ((uint8_t)0x02),
	STATE_OFFLINE				= ((uint8_t)0x03),
	STATE_PANIC					= ((uint8_t)0x04),
	STATE_DEACTIVATED		= ((uint8_t)0x05),
	STATE_INIT        	= ((uint8_t)0x06)
} system_state_t;

typedef struct {
	uint8_t hw_build_version;
	uint8_t sw_build_version;
} application_ctx_t;



#endif
