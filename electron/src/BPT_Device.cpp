#include "BPT_Device.h"


//uint16_t (*wiring_pins)[]; //size is determined by the device


/* GPS MT3339:
    Power to the GPS is controlled by a FET connected to D6
*/
uint16_t gps_mt[] = { D6 }; // power pin

external_device_t devices[EXTERNAL_DEVICE_COUNT] = {
  {
    EXTERNAL_DEVICE_MT3339, // id
    1,                      // version
    DEVICE_TYPE_GPS,        // type
    gps_mt,                 // wiring_pins
    2                       // pin count
  }
};
