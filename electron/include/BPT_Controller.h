#include "BPT.h"

// GPS module include
#ifdef EXTERNAL_DEVICE_MT3339
	#include "BPT_GPS_MT3339.h"
#else
	#include "BPT_GPS.h"
#endif

// Accelerometer include
#ifdef EXTERNAL_DEVICE_LIS3DH
	#include "BPT_Accel_LIS3DH.h"
#else
	#include "BPT_Accel.h"
#endif

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
	uint16_t numOfRetries;
	int lastSent;
	bool ackReceived;
} ack_event_t;


/*
	Maximum number of publish events to buffer
	The controller will slow down when the buffer fills up
*/
#define PUBLISH_EVENT_BUFFER_SIZE 10

/*
		Maximum number of ack events to wait for so the controller
		can re-republish them when necessary.
		The controller will slow down when this buffer fills up
*/
#define ACK_EVENT_BUFFER_SIZE 5

/*
	The maximum number of cloud publishes permitted at one time. This is
	restricted to particle.io's API
*/
#define MAX_SEQUENTIAL_PUBLISH 4

/*
	The maximum number of recent coordinates to track. This allows
	the controller to receive data from potentially multiple devices.
 */
#define MAX_REMOTE_GPS_COORDS 4

/*
  The maximum number of times to resend a published event
	that has not been acknowledged by a remote device. This
	applies only for events that require an ack such as
	a state change to the PANIC state. The delay between retries
	is specified by ACK_EVENT_RETRY_DELAY
*/
#define MAX_ACK_EVENT_RETRY 5

/*
	Wait x ms before resending the unacknowledged event.
  On the successive tries, wait x ** 2 up to x ** MAX_ACK_EVENT_RETRY ms
  on the last try. NB: be careful about integer overflow conditions
	when changing this property.
*/
#define ACK_EVENT_RETRY_DELAY 5000  /* 5e3 25e3 125e 625e ...*/


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

		// TODO: would it be better if allocation was dynamic?
		#ifdef EXTERNAL_DEVICE_MT3339
			BPT_GPS_MT3339 gpsModule;
		#else
			BPT_GPS gpsModule;
		#endif

		#ifdef EXTERNAL_DEVICE_LIS3DH
			BPT_Accel_LIS3DH accelModule;
		#else
			BPT_Accel accelModule;
		#endif

 	private:
		controller_mode_t cMode;   /* current controller mode */
		controller_state_t cState; /* current state */
		controller_state_t pState; /* previous state */
		int stateTime;

		// return the number of events process (if any)
		// this uses particle.io's cloud publishing services
		int _processPublishEvent();

		publish_event_t publishBuffer[PUBLISH_EVENT_BUFFER_SIZE];
		int publishEventCount = 0;

		ack_event_t ackEventBuffer[ACK_EVENT_BUFFER_SIZE];
		int ackEventCount = 0;


		remote_gps_coord_t remoteGpsCoord[MAX_REMOTE_GPS_COORDS];
		int remoteGpsIndex = 0; // index of the most recent received coordinate
};

#endif
