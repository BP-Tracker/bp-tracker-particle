#include "application.h"

#ifndef _BPT_Storage_h_
#define _BPT_Storage_h_

// typedef enum {
// 	PROP_RESERVED_SPACE   = ((uint16_t)0x0001)
// 	PROP_CONTROLLER_MODE  = ((uint16_t)0x0001),
// 	PROP_GEO_FENCE_RADIUS = (((uint16_t)0x0001)
// } system_property_t;


// typedef struct  {
// 	uint16_t hw_build_version;
// 	uint16_t sw_build_version;
// 	external_device_t *devices; //array of devices
// 	uint8_t device_count;
// } application_ctx_t;


typedef struct {
  //system_property_t id; // NB: this is an index
  uint16_t size;        // size must match the sizeof data being stored
  int hw_addr;
  bool (*validate)(void *value);
} system_storage_t;


class BPT_Storage {

  public:

    BPT_Storage();

    ~BPT_Storage();

    void getProperty(void); //TODO

};



#endif
