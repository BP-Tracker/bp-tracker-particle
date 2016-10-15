#ifndef _BPT_System_h_
#define _BPT_System_h_

typedef enum {
	CONTROLLER_MODE_NORMAL   	  = ((uint8_t)0x00), /*!< the default mode */
	CONTROLLER_MODE_DIAG    	  = ((uint8_t)0x01), /*!< for testing */
} controller_mode_t;



class BPT_System {

 public:

  BPT_System();

  void begin(void);

 private:
};


#endif
