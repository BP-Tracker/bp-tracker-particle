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
	int date;       		// the date and time coordinate was received
	uint8_t device; 		// remote device number
} remote_gps_coord_t;

/*
	The maximum number of recent coordinates to track. This allows
	the controller to receive data from potentially multiple devices.
 */
#define MAX_REMOTE_GPS_COORDS 4


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

		/*
			Receive a GPS coordinate from a remote device.
			When only one device will respond to events, the default device number
			is 1.
		*/
		bool receive(gps_coord_t *coord, uint8_t deviceNumber = 1);

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
		controller_mode_t cMode;
		controller_state_t cState;

		remote_gps_coord_t remoteGpsCoord[MAX_REMOTE_GPS_COORDS];
		int remoteGpsIndex; // index of the most recent received coordinate
};

#endif
