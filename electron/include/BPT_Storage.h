#include "application.h"

#ifndef _BPT_Storage_h_
#define _BPT_Storage_h_

class BPT;

// update this when new properties are added
#define NUM_APPLICATION_PROPERTIES 7

// NB: Do not change the order! (value is a index into an array)
typedef enum {
  PROP__RESERVED            = (0), /* do not use */
  PROP_CONTROLLER_VERSION   = (1), /* version of the app */
  PROP_CONTROLLER_MODE      = (2), /* */
  PROP_GEOFENCE_RADIUS      = (3), /* float > 0 */
  PROP_ACCEL_THRESHOLD      = (4), /* 0 - 16 */
  PROP_ACK_ENABLED          = (5), /* set false to disable ack */
  PROP_SLEEP_WAKEUP_STANDBY = (6) /* used when device is permitted to go into sleep. */
} application_property_t;

typedef enum {
  STORAGE_TYPE_CHAR    = (1),
  STORAGE_TYPE_INT     = (2),
  STORAGE_TYPE_UINT    = (3),
  STORAGE_TYPE_FLOAT   = (4),
  STORAGE_TYPE_BOOLEAN = (5),
  STORAGE_TYPE_AUTO    = (6), /* automatically determined*/
  STORAGE_TYPE_STRUCT  = (7) //TODO: how to handle these types?
} storage_type_t;

// application_property_t property;
typedef struct {
  storage_type_t type;
  int address;  // eeprom address
  uint8_t size;  // data byte size which can be greater than the actual data size
  uint8_t flags;
  BPT *owner; // can be null
} storage_t;

// storage_t flags
// [x,x,x,is_private,is_using_default_value,is_registered, is_size_exact,is_updateable]
#define STORAGE_FLAG_DEFAULTS           ((uint8_t)0x00)
#define STORAGE_FLAG_UPDATEABLE         ((uint8_t)0x01) /* can client update this */
#define STORAGE_FLAG_SIZE_NOT_EXACT     ((uint8_t)0x02)
#define STORAGE_FLAG_IS_REGISTERED      ((uint8_t)0x04) /* did a BPT class register this storage item? */
#define STORAGE_FLAG_HAS_DEFAULT_VALUE  ((uint8_t)0x08)
#define STORAGE_FLAG_IS_PRIVATE         ((uint8_t)0x10)

class BPT_Storage {

  public:

    BPT_Storage();

    ~BPT_Storage();

    bool getProperyStatus(application_property_t prop, uint8_t mask, bool any=false);

    void setPropertyStatus(application_property_t prop, uint8_t status);

    void clearPropertyStatus(application_property_t prop, uint8_t status);

    // _setSaved takes precedence
    bool isPropertySaved(application_property_t prop,
                          bool _setSaved=false, bool _clearSaved=false);

    BPT *getOwner(application_property_t prop);

    /**
     * NB: Does not support pointer types
     *
     * forceUpdate ignores the is_updateable flag - use with caution
     * _forceDefault overwrites the storage value with the default - use sparingly
     * TODO: support string types?
     */
    template<typename T>
    bool registerProperty(application_property_t prop, T defaultValue,
      BPT *owner, bool _forceDefault=false);

    template<typename T>
    bool getProperty(application_property_t prop, T& destination);

    // ignores the STORAGE_FLAG_UPDATEABLE flag
    template<typename T>
    bool setProperty(application_property_t prop, T& value);

    /**
     * Clears a single property.
     * Note this will automatically de-register the property and
     * subsequent calls to getProperty will return false.
     *
     * @param  prop the property to clear
     * @param  resetRegistered clears the registered status of the property
     * @return        true if the clear succeeded
     */
    bool clearProperty(application_property_t prop);

    /**
     * Resets the state of the class
     * @param  clearProperties clears all property value stored in EEPROM
     * @param  resetRegistered resets the registered flag of all properies
     */
    void reset(bool clearProperties, bool resetRegistered=false);

  private:
    storage_t *_getConfig(application_property_t prop);
};



/**
 * Inline templates
 * http://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file
 */
template<typename T>
bool BPT_Storage::getProperty(application_property_t prop, T& destination){

  storage_t *config = _getConfig(prop);
  if(!getProperyStatus(prop, STORAGE_FLAG_IS_REGISTERED)){
    return false;
  }

  EEPROM.get(config->address, destination);
  return true;
}

template<typename T>
bool BPT_Storage::setProperty(application_property_t prop, T& value){

  if(std::is_pointer<T>::value){
    return false;
  }

  storage_t *config = _getConfig(prop);
  if(sizeof(value) > config->size
      || getProperyStatus(prop, STORAGE_FLAG_IS_PRIVATE) ){
    return false;
  }

  EEPROM.put(config->address, value);
  clearPropertyStatus(prop, STORAGE_FLAG_HAS_DEFAULT_VALUE);

  return true;
}


template<typename T>
bool BPT_Storage::registerProperty(application_property_t prop, T defaultValue,
  BPT *owner, bool _forceDefault){

  if(std::is_pointer<T>::value || owner == 0){
    return false;
  }
  size_t size = sizeof(defaultValue);
  storage_t *config = _getConfig(prop);
  bool isRegistered = getProperyStatus(prop, STORAGE_FLAG_IS_REGISTERED);

  if(size > config->size ){
    return false; // data is too big
  }

  if(isRegistered && !_forceDefault){
    return true;
  }

  config->owner = owner;

  T temp;
  EEPROM.get(config->address, temp);
  bool hasDefault = memcmp(&temp, &defaultValue, size) == 0;

  Serial.printf("storage: register prop [prop=%u][hasDefault=%u]\n", 
      prop, hasDefault);

  if(hasDefault){
    setPropertyStatus(prop, STORAGE_FLAG_HAS_DEFAULT_VALUE);
  }

  if(!isPropertySaved(prop) || ( _forceDefault && !hasDefault )){
    Serial.printf("storage: writing to EEPROM [prop=%u][val=%i]\n", prop, defaultValue);
    EEPROM.put(config->address, defaultValue);

    isPropertySaved(prop, true);
    setPropertyStatus(prop, STORAGE_FLAG_HAS_DEFAULT_VALUE);
  }

  setPropertyStatus(prop, STORAGE_FLAG_IS_REGISTERED);
  return true;
}

#endif
