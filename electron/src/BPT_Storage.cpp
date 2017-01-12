#include "BPT_Storage.h"


// NB: index must match the value of application_property_t
// holds a maximum of 64 properties (assuming free space exists)
//
// when adding new properties, ensure the default value
// has the correct size when calling registerProperty
static storage_t storageConfig[NUM_APPLICATION_PROPERTIES] = { //NUM_APPLICATION_PROPERTIES TODO
  {
    // 0 PROP__RESERVED (reserved for BPT_Storage)
    // each bit position starting from the MSB
    // is a flag indicating whether the application property at that index
    // has saved data to EEPROM. Clearing this flag will effectively ignore
    // the data there
    STORAGE_TYPE_AUTO,
    0,            // eeprom address
    8,            // size in bytes
    STORAGE_FLAG_IS_REGISTERED & STORAGE_FLAG_IS_PRIVATE,
    0,            // BPT *owner
  },
  {
    // 1 PROP_CONTROLLER_VERSION
    STORAGE_TYPE_AUTO,
    8,           // eeprom address
    3,            // size uint8_t (major,mid,minor)
    STORAGE_FLAG_DEFAULTS,
    0,            // BPT *owner
  },
  {
    // 2 PROP_CONTROLLER_MODE
    STORAGE_TYPE_AUTO,
    11,            // eeprom address
    1,            // sizeof (controller_mode_t)
    STORAGE_FLAG_UPDATEABLE,
    0,            // BPT *owner
  },
  {
    // 3 PROP_GEOFENCE_RADIUS
    STORAGE_TYPE_FLOAT,
    12,            // eeprom address
    4,            // sizeof (float)
    STORAGE_FLAG_UPDATEABLE,
    0,            // BPT *owner
  },
  {
    // 4 PROP_ACCEL_THRESHOLD
    STORAGE_TYPE_UINT,
    16,            // eeprom address
    1,            // sizeof (uint8_t)
    STORAGE_FLAG_UPDATEABLE,
    0,            // BPT *owner
  },
  {
    // 5 PROP_ACK_ENABLED //TODO: updateable???
    STORAGE_TYPE_BOOLEAN,
    17,            // eeprom address
    1,            // sizeof (uint8_t bool)
    STORAGE_FLAG_UPDATEABLE,
    0,            // BPT *owner
  },
  {
    // 6 PROP_SLEEP_WAKEUP_STANDBY
    STORAGE_TYPE_INT,
    18,            // eeprom address
    4,            // sizeof (int)
    STORAGE_FLAG_UPDATEABLE,
    0,            // BPT *owner
  }
};


BPT_Storage::BPT_Storage(){ }

BPT_Storage::~BPT_Storage() {}

BPT *BPT_Storage::getOwner(application_property_t prop){
  storage_t *s = _getConfig(prop);
  return s->owner;
}

bool BPT_Storage::getProperyStatus(application_property_t prop, uint8_t mask, bool any){
  storage_t *s = _getConfig(prop);
  if(any){
    return (s->flags & mask) >= 1 ? true : false;
  }
  return (s->flags & mask) == mask ? true : false;
}

void BPT_Storage::setPropertyStatus(application_property_t prop, uint8_t status){
  storage_t *s = _getConfig(prop);
  s->flags |= status;
}

void BPT_Storage::clearPropertyStatus(application_property_t prop, uint8_t status){
  storage_t *s = _getConfig(prop);
  s->flags &= ~status;
}

bool BPT_Storage::clearProperty(application_property_t prop){
  if(prop < 1 || prop > NUM_APPLICATION_PROPERTIES){
    return false;
  }

  isPropertySaved(prop, false, true); //clear saved
  clearPropertyStatus(prop,
    STORAGE_FLAG_IS_REGISTERED & STORAGE_FLAG_HAS_DEFAULT_VALUE);
  return true;
}

// TODO: what to do by default?
void BPT_Storage::reset(bool clearProperties, bool resetRegistered){

  storage_t *s = _getConfig(PROP__RESERVED);

  if(clearProperties){
    Serial.printf("storage: resetting config blocks in EEPROM\n");
    for(int i = 0; i < s->size; i++){
      EEPROM.put(s->address + i, 0xFFFF);
    }
  }

  if(resetRegistered){
    for(int i = 1; i < NUM_APPLICATION_PROPERTIES; i++){
      application_property_t prop = static_cast<application_property_t>(i);
      clearPropertyStatus(prop,
        STORAGE_FLAG_IS_REGISTERED & STORAGE_FLAG_HAS_DEFAULT_VALUE);
    }
  }
}

bool BPT_Storage::isPropertySaved(application_property_t prop,
                                    bool _setSaved, bool _clearSaved){

  storage_t *s = _getConfig(PROP__RESERVED);
  uint8_t bitsPerBlock = 8;

  if(prop > s->size * bitsPerBlock){
    Serial.printf("storage: warning - property cannot be saved\n");
    return false;
  }

  // read the bit at position prop at address 0 (PROP__RESERVED)
  int byteBlock = prop / bitsPerBlock;
  uint8_t bytePosition = prop % bitsPerBlock;

  uint8_t byte = EEPROM.read( s->address + byteBlock );

  Serial.printf("storage: is prop saved [byte=%u][pos=%u]\n",
      byte, bytePosition);

  uint8_t mask = 0b10000000;
  mask = mask >> bytePosition;

  if(_setSaved || _clearSaved){
    uint8_t newByte = _setSaved ? byte & ~mask : byte | mask;

    Serial.printf("storge: writing saved flag to EEPROM [prop=%u][byte=%u]\n",
      prop, newByte);

    EEPROM.put(s->address + byteBlock, newByte);
    return _setSaved;
  }

  bool isSaved = (byte & mask) == mask ? false : true;
  return isSaved;
}


storage_t *BPT_Storage::_getConfig(application_property_t prop){
  if(prop < 0 || prop > NUM_APPLICATION_PROPERTIES){
    return nullptr;
  }
  return &storageConfig[prop];
}
