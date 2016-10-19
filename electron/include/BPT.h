#include "application.h"
#include "BPT_Device.h"
#include "BPT_Storage.h"

#ifndef _BPT_h_
#define _BPT_h_

// Global defines go here

// NB: number is a index into an array
// Do not change the order
typedef enum {
	PROP__RESERVED        = (0),
	PROP_CONTROLLER_MODE  = (1),
	PROP_GEO_FENCE_RADIUS = (2)
} application_property_t;

typedef enum {
	STATE_UNKNOWN       = ((uint8_t)0x00),
	STATE_ARMED         = ((uint8_t)0x01),
	STATE_DISARMED      = ((uint8_t)0x02),
	STATE_OFFLINE       = ((uint8_t)0x03),
	STATE_PANIC         = ((uint8_t)0x04),
	STATE_DEACTIVATED   = ((uint8_t)0x05),
	STATE_INIT          = ((uint8_t)0x06)
} system_state_t;


typedef struct  {
	uint16_t hw_build_version;
	uint16_t sw_build_version;
	external_device_t *devices; //array of devices
	uint8_t device_count;
} application_ctx_t;


typedef struct {
	float lat; // in degrees
	float lon; // in degrees
} gps_coord_t;

// calculations taken from http://movable-type.co.uk/scripts/latlong.html
typedef enum {
	HAVERSINE_FORMULA       = ((uint8_t)0x00),
	LAW_OF_COSINES_FORMULA  = ((uint8_t)0x01),
  EQUIRECT_APPROXIMATION  = ((uint8_t)0x02),
} distance_calc_t;

// base class
class BPT {

 	public:

  	BPT(application_ctx_t *applicationCtx);

		~BPT();

    bool registerProperty(application_property_t prop, BPT *owner);

		template<class T>
  	bool saveProperty(application_property_t prop, T& value);

		template<class T>
		T getProperty(application_property_t prop, T& defaultValue);

		static BPT_Storage storage;

		application_ctx_t *applicationCtx;
	private:

};


#endif
