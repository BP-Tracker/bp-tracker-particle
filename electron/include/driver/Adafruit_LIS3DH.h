#include "application.h"
#include "Adafruit_Sensor.h"

/**************************************************************************/
/*!
    @file     Adafruit_LIS3DH.h
    @author   K. Townsend / Limor Fried (Adafruit Industries)
    @license  BSD (see license.txt)

    This is a library for the Adafruit LIS3DH Accel breakout board
    ----> https://www.adafruit.com/products/????

    Adafruit invests time and resources providing this open source code,
    please support Adafruit and open-source hardware by purchasing
    products from Adafruit!

    @section  HISTORY

    v1.0  - First release
*/
/**************************************************************************/

 //#include "application.h"


//#include <Wire.h>
//#include <SPI.h>
//#include "Adafruit_Sensor.h"

/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
    #define LIS3DH_DEFAULT_ADDRESS  (0x18)    // if SDO/SA0 is 3V, its 0x19
/*=========================================================================*/

#define LIS3DH_REG_STATUS1       0x07
#define LIS3DH_REG_OUTADC1_L     0x08
#define LIS3DH_REG_OUTADC1_H     0x09
#define LIS3DH_REG_OUTADC2_L     0x0A
#define LIS3DH_REG_OUTADC2_H     0x0B
#define LIS3DH_REG_OUTADC3_L     0x0C
#define LIS3DH_REG_OUTADC3_H     0x0D
#define LIS3DH_REG_INTCOUNT      0x0E
#define LIS3DH_REG_WHOAMI        0x0F
#define LIS3DH_REG_TEMPCFG       0x1F
#define LIS3DH_REG_CTRL1         0x20
#define LIS3DH_REG_CTRL2         0x21
#define LIS3DH_REG_CTRL3         0x22
#define LIS3DH_REG_CTRL4         0x23
#define LIS3DH_REG_CTRL5         0x24
#define LIS3DH_REG_CTRL6         0x25
#define LIS3DH_REG_REFERENCE     0x26
#define LIS3DH_REG_STATUS2       0x27
#define LIS3DH_REG_OUT_X_L       0x28
#define LIS3DH_REG_OUT_X_H       0x29
#define LIS3DH_REG_OUT_Y_L       0x2A
#define LIS3DH_REG_OUT_Y_H       0x2B
#define LIS3DH_REG_OUT_Z_L       0x2C
#define LIS3DH_REG_OUT_Z_H       0x2D
#define LIS3DH_REG_FIFOCTRL      0x2E
#define LIS3DH_REG_FIFOSRC       0x2F
#define LIS3DH_REG_INT1CFG       0x30
#define LIS3DH_REG_INT1SRC       0x31
#define LIS3DH_REG_INT1THS       0x32
#define LIS3DH_REG_INT1DUR       0x33
#define LIS3DH_REG_CLICKCFG      0x38
#define LIS3DH_REG_CLICKSRC      0x39
#define LIS3DH_REG_CLICKTHS      0x3A
#define LIS3DH_REG_TIMELIMIT     0x3B
#define LIS3DH_REG_TIMELATENCY   0x3C
#define LIS3DH_REG_TIMEWINDOW    0x3D

typedef enum
{
  LIS3DH_RANGE_16_G         = 0b11,   // +/- 16g
  LIS3DH_RANGE_8_G           = 0b10,   // +/- 8g
  LIS3DH_RANGE_4_G           = 0b01,   // +/- 4g
  LIS3DH_RANGE_2_G           = 0b00    // +/- 2g (default value)
} lis3dh_range_t;


/* Used with register 0x2A (LIS3DH_REG_CTRL_REG1) to set bandwidth */
typedef enum
{
  LIS3DH_DATARATE_400_HZ     = 0b0111, //  400Hz
  LIS3DH_DATARATE_200_HZ     = 0b0110, //  200Hz
  LIS3DH_DATARATE_100_HZ     = 0b0101, //  100Hz
  LIS3DH_DATARATE_50_HZ      = 0b0100, //   50Hz
  LIS3DH_DATARATE_25_HZ      = 0b0011, //   25Hz
  LIS3DH_DATARATE_10_HZ      = 0b0010, // 10 Hz
  LIS3DH_DATARATE_1_HZ       = 0b0001, // 1 Hz
  LIS3DH_DATARATE_POWERDOWN  = 0,
  LIS3DH_DATARATE_LOWPOWER_1K6HZ  = 0b1000,
  LIS3DH_DATARATE_LOWPOWER_5KHZ  =  0b1001,

} lis3dh_dataRate_t;

typedef enum {
  LIS3DH_CTRL_REG1_ODR3 = 0x80,
  LIS3DH_CTRL_REG1_ODR2 = 0x40,
  LIS3DH_CTRL_REG1_ODR1 = 0x20,
  LIS3DH_CTRL_REG1_ODR0 = 0x10,
  LIS3DH_CTRL_REG1_LPEN = 0x08,
  LIS3DH_CTRL_REG1_ZEN = 0x04,
  LIS3DH_CTRL_REG1_YEN = 0x02,
  LIS3DH_CTRL_REG1_XEN = 0x01
} list3dh_ctrl_reg1_t;

typedef enum {
  LIS3DH_CTRL_REG2_HPM1 = 0x80,
  LIS3DH_CTRL_REG2_HPM0 = 0x40,
  LIS3DH_CTRL_REG2_HPCF2 = 0x20,
  LIS3DH_CTRL_REG2_HPCF1 = 0x10,
  LIS3DH_CTRL_REG2_FDS = 0x08,
  LIS3DH_CTRL_REG2_HPCLICK = 0x04,
  LIS3DH_CTRL_REG2_HPIS2 = 0x02,
  LIS3DH_CTRL_REG2_HPIS1 = 0x01
} list3dh_ctrl_reg2_t;


typedef enum {
  LIS3DH_CTRL_REG3_I1_CLICK = 0x80,
  LIS3DH_CTRL_REG3_I1_INT1 = 0x40,
  LIS3DH_CTRL_REG3_I1_DRDY = 0x10,
  LIS3DH_CTRL_REG3_I1_WTM = 0x04,
  LIS3DH_CTRL_REG3_I1_OVERRUN = 0x02
} list3dh_ctrl_reg3_t;

typedef enum {
  LIS3DH_CTRL_REG4_BDU = 0x80,
  LIS3DH_CTRL_REG4_BLE = 0x40,
  LIS3DH_CTRL_REG4_FS1 = 0x20,
  LIS3DH_CTRL_REG4_FS0 = 0x10,
  LIS3DH_CTRL_REG4_HR = 0x08,
  LIS3DH_CTRL_REG4_ST1 = 0x04,
  LIS3DH_CTRL_REG4_ST0 = 0x02,
  LIS3DH_CTRL_REG4_SIM = 0x01
} list3dh_ctrl_reg4_t;


typedef enum {
  LIS3DH_CTRL_REG5_BOOT = 0x80,
  LIS3DH_CTRL_REG5_FIFO_EN = 0x40,
  LIS3DH_CTRL_REG5_LIR_INT1 = 0x08,
  LIS3DH_CTRL_REG5_D4D_INT1 = 0x04
} list3dh_ctrl_reg5_t;

typedef enum {
  LIS3DH_CTRL_REG6_I2_CLICK = 0x80,
  LIS3DH_CTRL_REG6_I2_INT2 = 0x40,
  LIS3DH_CTRL_REG6_BOOT_I2 = 0x10,
  LIS3DH_CTRL_REG6_H_LACTIVE = 0x02
} list3dh_ctrl_reg6_t;

typedef enum {
  LIS3DH_INT1_CFG_AOI = 0x80,
  LIS3DH_INT1_CFG_6D = 0x40,
  LIS3DH_INT1_CFG_ZHIE_ZUPE = 0x20,
  LIS3DH_INT1_CFG_ZLIE_ZDOWNE = 0x10,
  LIS3DH_INT1_CFG_YHIE_YUPE = 0x08,
  LIS3DH_INT1_CFG_YLIE_YDOWNE = 0x04,
  LIS3DH_INT1_CFG_XHIE_XUPE = 0x02,
  LIS3DH_INT1_CFG_XLIE_XDOWNE = 0x01
} list3dh_ctrl_int1_cfg_t;

typedef enum {
  LIS3DH_INT1_SRC_IA = 0x40,
  LIS3DH_INT1_SRC_ZH = 0x20,
  LIS3DH_INT1_SRC_ZL = 0x10,
  LIS3DH_INT1_SRC_YH = 0x08,
  LIS3DH_INT1_SRC_YL = 0x04,
  LIS3DH_INT1_SRC_XH = 0x02,
  LIS3DH_INT1_SRC_XL = 0x01
} list3dh_ctrl_int2_src_t;

class Adafruit_LIS3DH : public Adafruit_Sensor {
 public:
  Adafruit_LIS3DH(void);
  Adafruit_LIS3DH(int8_t cspin);
  Adafruit_LIS3DH(int8_t cspin, int8_t mosipin, int8_t misopin, int8_t sckpin);

  bool       begin(uint8_t addr = LIS3DH_DEFAULT_ADDRESS);

  void read();
  uint16_t readADC(uint8_t a);

  void setRange(lis3dh_range_t range);
  lis3dh_range_t getRange(void);

  void setDataRate(lis3dh_dataRate_t dataRate);
  lis3dh_dataRate_t getDataRate(void);

  bool getEvent(sensors_event_t *event);
  void getSensor(sensor_t *sensor);

  uint8_t getOrientation(void);

  bool setupLowPowerWakeMode(uint8_t movementThreshold);
  uint8_t clearInterrupt();

  int16_t x, y, z;
  float x_g, y_g, z_g;


 private:
  uint8_t readRegister8(uint8_t reg);
  void writeRegister8(uint8_t reg, uint8_t value);
  uint8_t spixfer(uint8_t x = 0xFF);
  void beginTransaction();
  void endTransaction();

  int32_t _sensorID;
  int8_t  _i2caddr;

  // SPI
  int8_t _cs, _mosi, _miso, _sck;

  // LIS3DH INT1 is connected to this pin
  int intPin = WKP;
  uint8_t int1_cfg;
};
