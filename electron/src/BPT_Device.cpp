#include "BPT_Device.h"


/* GPS MT3339:
    Power to the GPS is controlled by a FET connected to D6
*/

// uses core defaults [pwr=user configured, usart-tx=?, usart-rx=?]
uint16_t gps_p[] = { D6 }; // [power pin]

// uses core defaults [ss=A2, sck=A3, miso=A4, mosi=A5, wakup=WKP]
// TODO: find a way to configure the driver pins using external_device_type_t
uint16_t accel_p[] = { }; // NB: A2 and WKP is hardcoded in BPT_Accel_LIS3DH

uint16_t asset_tracker_p[]  = {};

external_device_t devices[EXTERNAL_DEVICE_COUNT] = {
  {
    EXTERNAL_DEVICE_MT3339, // id - GPS module
    1,                      // version
    DEVICE_TYPE_GPS,        // type
    gps_p,                  // wiring_pins
    1                       // pin count
  },
  {
    EXTERNAL_DEVICE_LIS3DH, // id - Accelerometer module
    1,                      // version
    DEVICE_TYPE_ACCEL,      // type
    accel_p,                // wiring_pins
    0                      // pin count
  },
  {
    EXTERNAL_DEVICE_ASSET_TRACKER, // id - Assetracker shield
    1,                      // version
    DEVICE_TYPE_OTHER,      // type
    asset_tracker_p,        // wiring_pins
    0                       // pin count
  }
};
