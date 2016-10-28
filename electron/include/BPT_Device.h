#include "application.h"

#ifndef BPT_Device_h
#define BPT_Device_h

// External device configuration/wiring
// add configs for all external devices here
//  the value maps to an array index on devices (BPT_Devices.cpp)
#define EXTERNAL_DEVICE_MT3339 0
#define EXTERNAL_DEVICE_LIS3DH 1
#define EXTERNAL_DEVICE_ASSET_TRACKER 2

#define EXTERNAL_DEVICE_COUNT 3

typedef enum {
  DEVICE_TYPE_GPS            = ((uint8_t)0x01),
  DEVICE_TYPE_ACCEL          = ((uint8_t)0x02),
  DEVICE_TYPE_OTHER          = ((uint8_t)0x03)
} external_device_type_t;

// external hardware device configuration
typedef struct {
  uint16_t id;
  uint8_t version; // version or variant
  external_device_type_t type;
  uint16_t *wiring_pins; //size is determined by the device
  uint8_t pin_count;
} external_device_t;

#endif
