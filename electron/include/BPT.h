/**************************************************************************/
/*!
    @file     BPT.h (BP Tracker firmware)
    @author   Derek Benda
    @license  LPGL (see Licence.txt)

    v1.0  - First release
*/
/**************************************************************************/

#include "application.h"
#include "BPT_Device.h"
#include "BPT_Storage.h"

#ifndef _BPT_h_
#define _BPT_h_

// Global defines go here

/*
  NB: Do not change the order (value is a index into an array)
	These can be updated through the bpt:register" cloud function.
*/
typedef enum {
	PROP__RESERVED        = (0), /* do not use */
	PROP_CONTROLLER_MODE  = (1), /* */
	PROP_GEO_FENCE_RADIUS = (2), /* */
	PROP_ACCEL_THRESHOLD  = (3)  /* */
} application_property_t;

typedef enum {
	EVENT_STATE_CHANGE       = ((uint8_t)0x01), /* controller changed state */
	EVENT_REQUEST_GPS        = ((uint8_t)0x02), /* this ACK comes from a bpt:gps */
	EVENT_BATTERY_LOW 	     = ((uint8_t)0x03), /* requires ACK event */
	EVENT_NO_GPS_SIGNAL      = ((uint8_t)0x04),
	EVENT_SOFT_PANIC         = ((uint8_t)0x05), /* not enough data */
	EVENT_PANIC              = ((uint8_t)0x06), /* requires ACK event */
	EVENT_HARDWARE_FAULT     = ((uint8_t)0x07) /* TODO: can this be trapped */
} application_event_t;


typedef enum {
	STATE_BOOT_WAIT        = ((uint8_t)0x01),
	STATE_RESET            = ((uint8_t)0x02),
	STATE_RESET_WAIT       = ((uint8_t)0x03),
	STATE_INIT             = ((uint8_t)0x04),
	STATE_ONLINE           = ((uint8_t)0x05), //??
	STATE_ONLINE_WAIT      = ((uint8_t)0x06),
	STATE_ARMED            = ((uint8_t)0x07),
	STATE_DISARMED         = ((uint8_t)0x08),
	STATE_OFFLINE          = ((uint8_t)0x09),
	STATE_PANIC            = ((uint8_t)0x0A),
	STATE_DEACTIVATED      = ((uint8_t)0x0B),
	STATE_DEACTIVATED_WAIT = ((uint8_t)0x0C)
} controller_state_t;

typedef enum {
	CONTROLLER_MODE_NORMAL        = ((uint8_t)0x01), /*!< the default mode, pwoer saving */
	CONTROLLER_MODE_HIGH_SPEED    = ((uint8_t)0x02), /*!< always on  */
	CONTROLLER_MODE_SIM_AUTO      = ((uint8_t)0x03), /*!< testing - automatic simulation */
	CONTROLLER_MODE_SIM_MANUAL    = ((uint8_t)0x04) /*!< testing - manual simulation */
} controller_mode_t;


typedef struct  {
	uint16_t hw_build_version;
	uint16_t sw_build_version;
	external_device_t *devices; //array of devices
	uint8_t device_count;
	controller_mode_t mode;
} application_ctx_t;


typedef struct {
	float lat; // in degrees
	float lon; // in degrees
} gps_coord_t;

typedef struct {
	float x;
	float y;
	float z;
} accel_t;

// calculations taken from http://movable-type.co.uk/scripts/latlong.html
typedef enum {
	HAVERSINE_FORMULA       = ((uint8_t)0x01),
	LAW_OF_COSINES_FORMULA  = ((uint8_t)0x02),
  EQUIRECT_APPROXIMATION  = ((uint8_t)0x03),
} distance_calc_t;


// base class
class BPT {

 	public:

  	BPT(application_ctx_t *applicationCtx);

		~BPT();

    bool registerProperty(application_property_t prop, BPT *owner);

		//TODO
		// template<class T>
		// bool registerProperty(application_property_t prop, BPT *owner,
		// 	T minValue, T maxValue){
		// 		//TODO
		// 		return false;
		// } or use a function pointer to get the min/max?

		template<class T>
  	bool saveProperty(application_property_t prop, T& value){
			//TODO
			return false;
		}

		template<class T>
		T getProperty(application_property_t prop, T defaultValue){
		  //TODO
		  return defaultValue;
		}

		static BPT_Storage storage;
		application_ctx_t *applicationCtx;

	private:

};

#endif
