#ifndef MPL3115A2_H
#define MPL3115A2_H

#include <Arduino.h>
#include <Wire.h>

const uint8_t MPL3115A2_ADDRESS = 0x60;
const uint32_t MPL3115A2_TIMEOUT = 1000;

typedef enum {
  MPL3115A2_OK = 0,
  MPL3115A2_ERROR,
  MPL3115A2_STATUS_TIMEOUT,
  MPL3115A2_STATUS_NOT_IMPLEMENTED
} Mpl3115a2Error_enum;
typedef enum {
  MPL3115A2_STATUS = (uint8_t)0x00,
  MPL3115A2_OUT_P_MSB = (uint8_t)0x01,
  MPL3115A2_OUT_P_CSB = (uint8_t)0x02,
  MPL3115A2_OUT_P_LSB = (uint8_t)0x03,
  MPL3115A2_OUT_T_MSB = (uint8_t)0x04,
  MPL3115A2_OUT_T_LSB = (uint8_t)0x05,
  MPL3115A2_PT_DATA_CFG = (uint8_t)0x13,
  MPL3115A2_CTRL_REG1 = (uint8_t)0x26,
  MPL3115A2_CTRL_REG2 = (uint8_t)0x27,
  MPL3115A2_CTRL_REG3 = (uint8_t)0x28,
  MPL3115A2_CTRL_REG4 = (uint8_t)0x29,
  MPL3115A2_CTRL_REG5 = (uint8_t)0x30,
} Mpl3115a2Register_enum;
typedef enum { MPL3115A2_OS_MASK = (uint8_t)0x038 } Mpl3115a2RegisterMask_enum;
typedef enum {
  MPL3115A2_OS_6 = (uint8_t)0x00 << 3,
  MPL3115A2_OS_10 = (uint8_t)0x01 << 3,
  MPL3115A2_OS_18 = (uint8_t)0x02 << 3,
  MPL3115A2_OS_34 = (uint8_t)0x03 << 3,
  MPL3115A2_OS_66 = (uint8_t)0x04 << 3,
  MPL3115A2_OS_130 = (uint8_t)0x05 << 3,
  MPL3115A2_OS_258 = (uint8_t)0x06 << 3,
  MPL3115A2_OS_512 = (uint8_t)0x07 << 3,
  MPL3115A2_OS_DEFAULT = MPL3115A2_OS_6,
} Mpl3115a2OversamplingRatio_enum;
typedef enum {
  MPL3115A2_OS_6_DELAY = 6,
  MPL3115A2_OS_10_DELAY = 10,
  MPL3115A2_OS_18_DELAY = 18,
  MPL3115A2_OS_34_DELAY = 34,
  MPL3115A2_OS_66_DELAY = 66,
  MPL3115A2_OS_130_DELAY = 130,
  MPL3115A2_OS_258_DELAY = 258,
  MPL3115A2_OS_512_DELAY = 512,
  MPL3115A2_OS_DEFAULT_DELAY = MPL3115A2_OS_6_DELAY
} Mpl3115a2OversamplingRatioDelay_enum;
typedef enum { MPL3115A2_DATAREADY_CFG = 0x07 } Mpl3115a2PtDataCfg_enum;
typedef enum {
  MPL3115A2_CTRL_REG1_RESET = 0x00,
  MPL3115A2_CTRL_REG1_SET_BAROMETER = 0x01,
  MPL3115A2_CTRL_REG1_SET_ALTIMETER = 0x81
} Mpl3115a2CtrlReg1Mode_enum;

class MPL3115A2 {
private:
  TwoWire *_i2cHandler;
  bool initSensor();
  Mpl3115a2OversamplingRatio_enum _oversamplingRatio = MPL3115A2_OS_DEFAULT;
  uint8_t readRegister(uint8_t registerAddress, uint8_t *readData);
  uint8_t readRegister(uint8_t registerAddress, uint8_t *readData, int size);
  uint8_t writeRegister(uint8_t registerAddress, uint8_t data);

public:
  MPL3115A2();
  MPL3115A2(TwoWire *i2cHandler);
  ~MPL3115A2();
  bool init();
  bool init(TwoWire *i2cHandler);
  void setOversamplingRatio(Mpl3115a2OversamplingRatio_enum oversamplingRatio);
  Mpl3115a2OversamplingRatio_enum getOverSamplingRatio();
  Mpl3115a2Error_enum setModeBarometer();
  Mpl3115a2Error_enum setModeAltimeter();
  float getPressure();
  Mpl3115a2Error_enum getPressure(float &pressure);
  float getAltitude();
  Mpl3115a2Error_enum getAltitude(float &altitude);
  void printStatus(Mpl3115a2Error_enum status);
};

#endif // MPL3115A2_H