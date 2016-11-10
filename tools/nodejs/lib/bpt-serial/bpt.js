'use strict'

var BPT = function (){
};

BPT.prototype = {

  /* controller states */
  // TODO: update values
  STATES: {
    STATE_OFFLINE              : 0x01,
    STATE_DEACTIVATED          : 0x02,
    STATE_RESET                : 0x03,
    STATE_ARMED                : 0x04,
    STATE_DISARMED             : 0x05,
    STATE_PANIC                : 0x06,
    STATE_PAUSED               : 0x07, // for testing
    STATE_RESUMED              : 0x08, // for testing

    INTERNAL_STATE_ACTIVATED   : 0x09,
    INTERNAL_STATE_SOFT_PANIC  : 0x0A,
    INTERNAL_STATE_ONLINE_WAIT : 0x0B,
    INTERNAL_STATE_RESET_WAIT  : 0x0C,
    INTERNAL_STATE_SLEEP       : 0x0D
  },


  /* test_input_t types */
  TEST_INPUTS: {
    TEST_INPUT_GPS      : 1,
    TEST_INPUT_AUTO_GPS : 2,
    TEST_INPUT_ACCEL_INT: 3
  },

 /* application_property_t types */
  APPLICATION_PROPERTIES: {
    PROP_CONTROLLER_MODE : 1,
    PROP_GEOFENCE_RADIUS : 2,
    PROP_ACCEL_THRESHOLD : 3,
    PROP_ACK_ENABLED     : 4
  },


  /* application_event_t events */
  EVENTS: {
    EVENT_STATE_CHANGE     : 0x01,
    EVENT_REQUEST_GPS      : 0x02,
    EVENT_BATTERY_LOW 	   : 0x03,
    EVENT_NO_GPS_SIGNAL    : 0x04,
    EVENT_SOFT_PANIC       : 0x05,
    EVENT_PANIC            : 0x06,
    EVENT_PROBE_CONTROLLER : 0x07,
    EVENT_TEST             : 0x08,
    EVENT_SERIAL_COMMAND   : 0x09,
    EVENT_ERROR            : 0x0A,
    EVENT_HARDWARE_FAULT   : 0x0B,
  },

  /* controller modes */
  CONTROLLER_MODES: {
    CONTROLLER_MODE_NORMAL    : 0x01,
    CONTROLLER_MODE_HIGH_SPEED: 0x02,
    CONTROLLER_MODE_TEST      : 0x03,
  }
};

module.exports = BPT;
