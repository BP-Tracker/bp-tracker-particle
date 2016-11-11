/**************************************************************************/
/*!
    @file     BPT.h (BP Tracker firmware)
    @author   Derek Benda
    @license  MIT (see Licence.txt)

    v0.5  - First release
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
  PROP__RESERVED            = (0), /* do not use */
  PROP_CONTROLLER_MODE      = (1), /* */
  PROP_GEOFENCE_RADIUS      = (2), /* */
  PROP_ACCEL_THRESHOLD      = (3), /* */
  PROP_ACK_ENABLED          = (4), /* set false to disable ack */
  PROP_SLEEP_WAKEUP_STANDBY = (5), /* used when device is permitted to go into sleep. */
} application_property_t;

/* test data types for the controller in the CONTROLLER_MODE_TEST mode */
typedef enum {
  TEST_INPUT_GPS        = (1), /* sets the GPS coordinate of device. format: lat,lon */
  TEST_INPUT_AUTO_GPS   = (2), /* TODO: like TEST_INPUT_GPS expect the coordinate is arbitrarily chosen */
  TEST_INPUT_ACCEL_INT  = (2)  /* TODO: triggers a 'wake' event on the accelerometer */
} test_input_t;


// bpt:event event codes
typedef enum {
  EVENT_STATE_CHANGE       = ((uint8_t)0x01), /* controller changed state */
  EVENT_REQUEST_GPS        = ((uint8_t)0x02), /* TODO: can this ack also come from a bpt:gps event */
  EVENT_BATTERY_LOW 	     = ((uint8_t)0x03), /* requires ACK event */
  EVENT_NO_GPS_SIGNAL      = ((uint8_t)0x04), /* data includes the age of the last known coord */
  EVENT_SOFT_PANIC         = ((uint8_t)0x05), /* not enough data to determine panic state */
  EVENT_PANIC              = ((uint8_t)0x06), /* requires ACK event */
  EVENT_PROBE_CONTROLLER   = ((uint8_t)0x07), /* this is a special event a remote device can send to probe the controller */
  EVENT_TEST               = ((uint8_t)0x08), /* when the controller is in test mode, all btp:event's use this code */
  EVENT_SERIAL_COMMAND     = ((uint8_t)0x09), /* a command was received via the serial interface */
  EVENT_ERROR              = ((uint8_t)0x0A), /* TODO: triggered when...  */
  EVENT_HARDWARE_FAULT     = ((uint8_t)0x0B) /* TODO: can this be trapped? */
} application_event_t;


#define NUM_CONTROLLER_STATES 13 /* NB: update this when states are added/removed */

/* the controller_state_t number that begins the internal states */
#define INTERNAL_STATES_INDEX 9

// NB: The values need to be sequential beginning at 1
typedef enum {
  /* Public states */
  STATE_OFFLINE          = ((uint8_t)0x01),
  STATE_DEACTIVATED      = ((uint8_t)0x02),
  STATE_RESET            = ((uint8_t)0x03),
  STATE_ARMED            = ((uint8_t)0x04),
  STATE_DISARMED         = ((uint8_t)0x05),
  STATE_PANIC            = ((uint8_t)0x06),
  STATE_PAUSED           = ((uint8_t)0x07), // for testing
  STATE_RESUMED          = ((uint8_t)0x08), // for testing

  /* Private states - these should not be set from the cloud/client. See BPT_Controller::setState. */
  STATE_ACTIVATED        = ((uint8_t)0x09),
  STATE_SOFT_PANIC       = ((uint8_t)0x0A),
  STATE_ONLINE_WAIT      = ((uint8_t)0x0B),
  STATE_RESET_WAIT       = ((uint8_t)0x0C),
  STATE_SLEEP            = ((uint8_t)0x0D)
} controller_state_t;

typedef enum {
  /*
    The default mode, uses accelerometer for power saving
  */
  CONTROLLER_MODE_NORMAL        = ((uint8_t)0x01),

  /*
    Always on, disables accelerometer, GPS data is polled as
    fast as possible.
   */
  CONTROLLER_MODE_HIGH_SPEED    = ((uint8_t)0x02),

  /*
    Puts the controller into testing mode to permit mocking
    states such as the device's GPS coordinates and wake modes.

    In this mode all bpt:event event are of type EVENT_TEST
  */
  CONTROLLER_MODE_TEST          = ((uint8_t)0x03),
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


    // template<class T> //TODO
    // bool registerProperty(application_property_t prop, BPT *owner,
    // 	T minValue, T maxValue){
    // 		return false;
    // } or use a function pointer to get the min/max?

    template<class T>
    bool saveProperty(application_property_t prop, T& value){ //TODO

      return false;
    }

    template<class T>
    T getProperty(application_property_t prop, T defaultValue){ //TODO

      return defaultValue;
    }

    static BPT_Storage storage;
    application_ctx_t *applicationCtx;

  private:

};

#endif
