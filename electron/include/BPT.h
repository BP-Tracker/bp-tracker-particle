#include "application.h"

#ifndef _BPT_h_
#define _BPT_h_

// Global defines go here




// remove or add attached external devices here (value is the version/variant)
#define EXTERNAL_DEVICE_MT3339 1
#define EXTERNAL_DEVICE_LIS3DH 1
#define EXTERNAL_DEVICE_ASSET_TRACKER 2



// all external device configuration attached to the electron
typedef struct {


} external_device_t;

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
	uint16_t hw_build_version;
	uint16_t sw_build_version;
} application_ctx_t;

typedef struct {
	float lat; // in degrees
	float lon; // in degrees
} gps_coord_t;

// calculations taken from http://movable-type.co.uk/scripts/latlong.html
typedef enum {
	HAVERSINE_FORMULA   	  = ((uint8_t)0x00),
	LAW_OF_COSINES_FORMULA	= ((uint8_t)0x01),
  EQUIRECT_APPROXIMATION  = ((uint8_t)0x02),
} distance_calc_t;



#endif
