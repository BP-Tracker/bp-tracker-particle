#include "BPT.h"
#include "BPT_GPS.h"

#ifndef _BPT_Controller_h_
#define _BPT_Controller_h_


// #define CONTROLLER_MODE_NORMAL 			1 /*!< the default mode */
// #define CONTROLLER_MODE_DIAG 				2 /*!< for testing */
// #define CONTROLLER_MODE_AUTO_SIM  	3 /*!< automatic simulation */
// #define CONTROLLER_MODE_MANUAL_SIM  4 /*!< manual simulation */

typedef enum {
	CONTROLLER_MODE_NORMAL   	  	= ((uint8_t)0x00), /*!< the default mode, pwoer saving */
	CONTROLLER_MODE_HIGH_SPEED  	= ((uint8_t)0x01), /*!< always on  */
	CONTROLLER_MODE_SIM_AUTO      = ((uint8_t)0x02), /*!< testing - automatic simulation */
	CONTROLLER_MODE_SIM_MANUAL    = ((uint8_t)0x03) /*!< testing - manual simulation */
} controller_mode_t;


class BPT_Controller {

 	public:

  	BPT_Controller(application_ctx_t *a);

    void setup();

  	void reset(void);

		bool setMode(controller_mode_t mode);
		controller_mode_t getMode();

		bool setState(system_state_t state);
		system_state_t getState();

		void loop(void);

    BPT_GPS gps;

 	private:
    application_ctx_t *applicationCtx;
    controller_mode_t controllerMode;
		system_state_t systemState;

};

#endif
