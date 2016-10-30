#include "BPT.h"

#include "BPT_GPS.h"
#include "BPT_Accel.h"

#ifndef _BPT_Controller_h_
#define _BPT_Controller_h_

typedef struct {
  gps_coord_t coord;
  int datetime;       // date and time the coordinate was received
  uint8_t device; 		// remote device number
} remote_gps_coord_t;

typedef struct {
  application_event_t event;
  uint8_t deviceNum; // 0 -> N/A or any device
  int datetime;
  char data[15]; //TODO: too small?
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
#define CONTROLLER_FREQUENCY 1000

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
  is specified by ACK_EVENT_RETRY_DELAY
*/
#define MAX_ACK_EVENT_RETRY 5

/*
  Wait x sec before resending the unacknowledged event.
  On the successive tries, wait x ** 2 up to x ** MAX_ACK_EVENT_RETRY ms
  on the last try. NB: be careful about integer overflow conditions
  when changing this property.
*/
#define ACK_EVENT_RETRY_DELAY 5  /* 5sec 25sec 125sec 625e ...*/


class BPT_Controller: public BPT {

  public:

    BPT_Controller(application_ctx_t *applicationCtx);

    void setup();

    void reset(void);

    bool setMode(controller_mode_t m);
    controller_mode_t getMode();

    bool setState(controller_state_t s);
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
      the data format [deviceNum:]application_event_t[,data1, data2, ...]

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
        uint8_t forDeviceNum = 0, int _numOfFreeSlots = 1);


    bool getGpsCoord(gps_coord_t *c);

    int getAcceleration(accel_t *t);

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
    unsigned long controllerStateTime;
    unsigned long publishTime;
    unsigned long publishAckTime;


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
    bool ackEventsEnabled;

    // sets t to millis()
    void _resetTime(unsigned long *t);

    // FIFO queue
    publish_event_t publishEventQueue[PUBLISH_EVENT_BUFFER_SIZE];
    int publishEventFront;

    ack_event_t ackEventBag[ACK_EVENT_BUFFER_SIZE];

    remote_gps_coord_t remoteGpsCoord[MAX_REMOTE_GPS_COORDS];
    int remoteGpsIndex; // index of the most recent received coordinate
};

#endif
