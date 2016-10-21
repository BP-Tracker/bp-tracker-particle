#include "BPT.h"

// GPS module include
#ifdef EXTERNAL_DEVICE_MT3339
	#include "BPT_GPS_MT3339.h"
#else
	#include "BPT_GPS.h"
#endif

#ifndef _BPT_Controller_h_
#define _BPT_Controller_h_


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

		bool getGpsCoord(gps_coord_t *c);

		// TODO: would it be better if allocation was dynamic?
		#ifdef EXTERNAL_DEVICE_MT3339
			BPT_GPS_MT3339 gpsModule;
		#else
			BPT_GPS gpsModule;
		#endif

 	private:
		controller_mode_t cMode;
		controller_state_t cState;
};

#endif
