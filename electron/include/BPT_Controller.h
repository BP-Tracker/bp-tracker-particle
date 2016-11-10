#include "BPT.h"

#include "BPT_GPS.h"
#include "BPT_Accel.h"

#ifndef _BPT_Controller_h_
#define _BPT_Controller_h_

#define MAX_PUBLISH_DATA_LEN 64 /* TODO: too small? */

typedef struct {
  gps_coord_t coord;
  int datetime;       // date and time the coordinate was received
  uint8_t device; 		// remote device number
} remote_gps_coord_t;

typedef struct {
  application_event_t event;
  uint8_t deviceNum; // 0 -> N/A or any device
  int datetime;
  char data[MAX_PUBLISH_DATA_LEN];
  int retryCount; // contains 0 when ackRequired is false, otherwise the current resubmit attempt
} publish_event_t;

typedef struct {
  publish_event_t publishEvent;
  uint16_t publishCount;
  bool ackNotReceived; // reverse logic so zeroing the struct frees it
  bool publishFailure; // event not received after MAX_ACK_EVENT_RETRY attempts
  int lastPublish;
} ack_event_t;



/*
  Maximum number of publish events to buffer
  The controller will slow down when the buffer fills up
*/
#define PUBLISH_EVENT_BUFFER_SIZE 10

/*
    Maximum number of ack events to wait for so the controller
    can republish them when necessary.
    The controller will slow down when this buffer fills up.
    See also CHECK_ACK_QUEUE_FREQUENCY.
*/
#define ACK_EVENT_BUFFER_SIZE 5

/*
  The minimum time (ms) before processing the publish queue.

  NB: this should be at least one second because the particle.io API
  stipulates a max publish frequency of 1 event/sec (a bust of 4 events at
  one time is allowed but requires 4 second delay afterwords)
*/
#define CHECK_PUBLISH_QUEUE_FREQUENCY 2000

/*
  How often to execute the controller's main logic in ms
  when it's awake. Gauge this for power utilization TODO: fine tune this
*/
#define CONTROLLER_RUN_FREQUENCY 1000

/*
  The maximum number of cloud publishes permitted at one time. This is
  restricted to particle.io's API
*/
#define MAX_SEQUENTIAL_PUBLISH 4

/*
  The time to wait in ms before publishing another set of events
*/
#define SEQUENTIAL_PUBLISH_COOLDOWN 5000

/*
  The minimum elepased time in ms before processing
  the ack event queue
*/
#define CHECK_ACK_QUEUE_FREQUENCY 5000

/*
  The maximum number of recent coordinates to track. This allows
  the controller to receive data from potentially multiple devices.
 */
#define MAX_REMOTE_GPS_COORDS 4

/*
  The maximum age in seconds a coordinate from a remote
  device can be considered recent.
*/
#define GPS_COORD_MAX_AGE 120

/*
  The maximum number of times to resend a published event
  that has not been acknowledged by a remote device. This
  applies only for events that require an ack such as
  a state change to the PANIC state. The delay between retries
  is specified by ACK_EVENT_RETRY_DELAY TODO: tune later
*/
#define MAX_ACK_EVENT_RETRY 3

/*
  Wait x sec before resending the unacknowledged event.
  On the successive tries, wait x ** 2 up to x ** MAX_ACK_EVENT_RETRY ms
  on the last try. NB: be careful about integer overflow conditions
  when changing this property.
*/
#define ACK_EVENT_RETRY_DELAY 5  /* 5sec 25sec 125sec 625e ...*/

/*
  The time in ms to wait in the RESET_WAIT_STATE into ONLINE_STATE
*/
#define RESET_WAIT_STATE_DELAY 5000

/*
  The amount of time to wait (in sec) for for GPS data before giving up
  and entering STATE_SOFT_PANIC. TODO: tune later
  10 mins = 60 * 10 = 600
*/
#define REQUEST_GPS_TIMEOUT 120

/*
  The amount of time to wait (in sec) for a GPS acquisition on
  the device when required before gonig into an OFFLINE state
  10 hours = 10 * 60 * 60 = 36000
*/
#define GPS_ACQUISITION_TIMEOUT 36000

// Geofence radius in meters
#define DEFAULT_GEOFENCE_RADIUS 500

/*
  The amount of time (in sec) the device is stationary
  before automatically moving the to ARMED state.
  1 min = 60 sec
*/
#define AUTO_ARM_AFTER_IDLE_PERIOD 60

/*
  Enables the sleep state and the controller automatically wakes up
  after this time (in sec) has elsaped without any movement detection
  4 hr = 4 * 60 * 60 = 14400
  Set to 0 to disable
*/
// #define AUTO_WAKE_AFTER_SLEEP_PERIOD 14000 TODO: tune SLEEP_STATE_PERIOD
#define SLEEP_STATE_PERIOD 30

/*
  The maximum time (in sec) the contoller will spend in the SOFT_PANIC state without
  receiving any remote communications before going into the deep sleep
  (OFFLINE) state. Setting to 0 disables this. Max value is ~ 49 days
  (the max long value returned from millis) TODO: tune later
*/
#define SOFT_PANIC_TO_OFFLINE_PERIOD 300

/*
  Same logic as SOFT_PANIC_TO_OFFLINE_PERIOD except applying to the PANIC state
 */
#define PANIC_TO_OFFLINE_PERIOD 300


/*
  The maximum time (in sec) the controller will stay paused before
  automatically resuming. Set to 0 to allow an indefinite time until
  the controller recieves input form a remote device
  2 hr = 2 * 60 * 60 = 7200 TODO: change it
*/
#define MAX_PAUSED_STATE_PERIOD 0

/*
  Automatically send GPS coordinates at this frequency (in sec) in the
  PANIC state. Set zero to disable. Use values higher than 1 sec. TODO: tune this
*/
#define PANIC_GPS_PUBLISH_FREQUENCY 60

/*
  The max number of GPS publish events in the panic state
*/
#define MAX_PANIC_GPS_PUBLISH_EVENTS 5

/*
  Wait this time (in sec) before throwing a CONTROLLER_ERROR event.
  This is used in some states the controller can be stuck in.
  5 mins = 5 * 60 = 300
  TODO: tune this parameter
*/
#define CONTROLLER_ERROR_TIMEOUT 300

/*
  Maximum length of the exception message. Longer messages
  are truncated to this amount
 */
#define MAX_EXCEPTION_MSG_LENGTH 64

class BPT_Controller: public BPT {

  public:

    BPT_Controller(application_ctx_t *applicationCtx);

    void setup();

    void reset(void);

    bool setMode(controller_mode_t m);
    controller_mode_t getMode();

    // _force, _resetStateTime and _reserveSlots is used by the controller
    bool setState(controller_state_t s,
      bool _force = false, int _reserveSlots = 1, bool _resetStateTime = true);

    controller_state_t getState();

    void loop(void);

    /* Receive a GPS coordinate from a remote device.
      The default device number is 1 when only one device
      will respond to events.
    */
    bool receive(gps_coord_t *coord, uint8_t deviceNumber);

    /* Receive an event acknowledgment from a remote device */
    bool receive(application_event_t e, const char *data, uint8_t deviceNumber);

    /*
      Publishes the event to the cloud via "bpt:event" with
      the format: [deviceNum:]application_event_t,ack_retry[,data1, data2, ...]

      When ack_retry is non zero, the controller expects a bpt:ack reply for
      that event. The number indicates the n'th resubmit attempt.
      See also MAX_ACK_EVENT_RETRY.

      Returns true if the event was sucessfully added to the queue
      for publishing.

      Use _numOfFreeSlots if you need to publish more than one event
      atomically. The function will return false if the slots
      are unavailable. As a convience, the controller should always have
      at least one (maybe two?) free slots in a loop cycle. That way
      _publishEvent will always succeed.

      Set forDeviceNum when the event is indended for a specific device.
      usually used with ackRequired.
    */
    bool publish(application_event_t event,
        const char *data, bool ackRequired = false,
        uint8_t forDeviceNum = 0, int _numOfFreeSlots = 1, int _ackRetryNumber = 0);


    int getGpsCoord(gps_coord_t *c);

    int getAcceleration(accel_t *t);

    bool hasException();

    const char* getException(bool reset = false);

    BPT_GPS gpsModule;
    BPT_Accel accelModule;

    // keep track of the number of events that failed to get an ACK
    // after MAX_ACK_EVENT_RETRY retries
    static int totalDroppedAckEvents;

    static int totalPublishedEvents; // total events published since uptime


    //TODO: move to private later
    int publishEventCount;
    int ackEventCount;

    //TODO: remove later
    //unsigned long publishTest[PUBLISH_EVENT_BUFFER_SIZE];

   private:
    controller_mode_t cMode;   /* current controller mode */
    controller_state_t cState; /* current state */
    controller_state_t pState; /* previous state */
    unsigned long _stateTime;
    unsigned long _publishTime;
    unsigned long _publishAckTime;


    // returns the number of events processed and published (if any)
    // using particle.io's cloud services
    int _processPublishEvent();

    // looks in ackEventBag for events that haven't yet
    // received an acknowledgment after a period of time
    // and resubmits them for publishing
    int _processAckEvent();

    // publishes a bpt:event and forces out pending ack events
    void _probeController(uint8_t deviceNum);

    // set to false if any events set by the controller
    // doesn't need to be acknowledged.
    // default is true see PROP_ACK_ENABLED
    bool _ackEventsEnabled;

    // sets t to millis()
    void _resetTime(unsigned long *t);

    void _checkPublishQueue();
    void _checkAckQueue();

    // returns and logs the controller message: TODO
    void _logException(const char *msg);
    bool _hasException;
    char _exceptionMessage[MAX_EXCEPTION_MSG_LENGTH];

    // FIFO queue
    publish_event_t _publishEventQueue[PUBLISH_EVENT_BUFFER_SIZE];
    int _publishEventFront;

    ack_event_t _ackEventBag[ACK_EVENT_BUFFER_SIZE];

    remote_gps_coord_t _remoteGpsCoord[MAX_REMOTE_GPS_COORDS];
    int _remoteGpsIndex; // index of the most recent received coordinate
    bool _requestGpsSent;

    float _geoFenceRadius; // in meters
    bool _wakeDetectionConfigured; // setup a movement interrupt for the disarmed state

    controller_state_t _resumePreviousState; // for STATE_PAUSED/STATE_RESUMED

    int _panicGpsPublishEventCount;

    // tracks the last time a remote device communicated with the
    // controller thougth the receive functions
    int _lastRemoteCommunication;
    int _lastRemoteDeviceNum;

    // the time the controller entered the SOFT_PANIC state
    int _softPanicStartTime;
};

#endif
