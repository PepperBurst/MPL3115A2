#include "./MPL3115A2.h"

MPL3115A2::MPL3115A2() {}

MPL3115A2::MPL3115A2(TwoWire *i2cHandler) { _i2cHandler = i2cHandler; }

MPL3115A2::~MPL3115A2() {}

bool MPL3115A2::init() { return (this->initSensor()); }

bool MPL3115A2::init(TwoWire *i2cHandler) {
  _i2cHandler = i2cHandler;
  return (initSensor());
}

bool MPL3115A2::initSensor() {
  _i2cHandler->beginTransmission(MPL3115A2_ADDRESS);
  uint8_t reply = _i2cHandler->endTransmission();
  if (reply == I2C_ERROR_OK) {
#ifdef DEBUG
    Serial.println(F("MPL3115A2 found!"));
#endif // DEBUG
    return (true);
  }
#ifdef DEBUG
  Serial.println(F("MPL3115A2 not found!"));
#endif // DEBUG
  return (false);
}

uint8_t MPL3115A2::readRegister(uint8_t registerAddress, uint8_t *readData) {
  _i2cHandler->beginTransmission(MPL3115A2_ADDRESS);
  _i2cHandler->write(registerAddress);
  uint8_t status = _i2cHandler->endTransmission();
  _i2cHandler->requestFrom(MPL3115A2_ADDRESS, (uint8_t)1);
  if (_i2cHandler->available()) {
    uint8_t data = _i2cHandler->read();
    *readData = data;
  }
  return (status);
}

uint8_t MPL3115A2::readRegister(uint8_t registerAddress, uint8_t *readData,
                                int size) {
  _i2cHandler->beginTransmission(MPL3115A2_ADDRESS);
  _i2cHandler->write(registerAddress);
  uint8_t status = _i2cHandler->endTransmission();
  _i2cHandler->requestFrom(MPL3115A2_ADDRESS, (uint8_t)size);
  int index = 0;
  while (_i2cHandler->available()) {
    uint8_t data = _i2cHandler->read();
    readData[index] = data;
    index++;
  }
  return (status);
}

uint8_t MPL3115A2::writeRegister(uint8_t registerAddress, uint8_t data) {
  _i2cHandler->beginTransmission(MPL3115A2_ADDRESS);
  _i2cHandler->write(registerAddress);
  _i2cHandler->write(data);
  uint8_t status = _i2cHandler->endTransmission();
  return (status);
}

void MPL3115A2::setOversamplingRatio(
    Mpl3115a2OversamplingRatio_enum oversamplingRatio) {
  _oversamplingRatio = oversamplingRatio;
}

Mpl3115a2OversamplingRatio_enum MPL3115A2::getOverSamplingRatio() {
  return (_oversamplingRatio);
}

Mpl3115a2Error_enum MPL3115A2::setModeBarometer() {
  uint8_t status;
  status = this->writeRegister(MPL3115A2_CTRL_REG1, MPL3115A2_CTRL_REG1_RESET);
  if (status != I2C_ERROR_OK) {
    return (MPL3115A2_ERROR);
  }
  status = this->writeRegister(MPL3115A2_PT_DATA_CFG, MPL3115A2_DATAREADY_CFG);
  if (status != I2C_ERROR_OK) {
    return (MPL3115A2_ERROR);
  }
  uint8_t registerValue =
      MPL3115A2_CTRL_REG1_SET_BAROMETER & ~(MPL3115A2_OS_MASK);
  registerValue |= (uint8_t)this->getOverSamplingRatio();
  status = this->writeRegister(MPL3115A2_CTRL_REG1, registerValue);
  if (status != I2C_ERROR_OK) {
    return (MPL3115A2_ERROR);
  }
  return (MPL3115A2_OK);
}

Mpl3115a2Error_enum MPL3115A2::setModeAltimeter() {
  uint8_t status;
  status = this->writeRegister(MPL3115A2_CTRL_REG1, MPL3115A2_CTRL_REG1_RESET);
  if (status != I2C_ERROR_OK) {
    return (MPL3115A2_ERROR);
  }
  status = this->writeRegister(MPL3115A2_PT_DATA_CFG, MPL3115A2_DATAREADY_CFG);
  if (status != I2C_ERROR_OK) {
    return (MPL3115A2_ERROR);
  }

  uint8_t registerValue =
      MPL3115A2_CTRL_REG1_SET_ALTIMETER & ~(MPL3115A2_OS_MASK);
  registerValue |= (uint8_t)this->getOverSamplingRatio();
  status = this->writeRegister(MPL3115A2_CTRL_REG1, registerValue);
  if (status != I2C_ERROR_OK) {
    return (MPL3115A2_ERROR);
  }
  return (MPL3115A2_OK);
}

float MPL3115A2::getPressure() {
  uint8_t dataBuffer[4] = {0};
  uint8_t dataReady = 0;
  uint8_t status = 0;
  uint32_t rawPressure = 0;
  float pressure = 0;
  uint32_t timeout = millis();
  while (!(dataReady & 0x08)) {
    if ((millis() - timeout) > MPL3115A2_TIMEOUT) {
#ifdef DEBUG
      Serial.println(F("Sensor timeout!"));
#endif // DEBUG
      return (0);
    }
    status = this->readRegister(MPL3115A2_STATUS, &dataReady);
    if (status != I2C_ERROR_OK) {
#ifdef DEBUG
      Serial.println(F("I2C error!"));
#endif // DEBUG
      return (0);
    }
  }
  status = this->readRegister(MPL3115A2_STATUS, dataBuffer, sizeof(dataBuffer));
  if (status != I2C_ERROR_OK) {
#ifdef DEBUG
    Serial.println(F("I2C error!"));
#endif // DEBUG
    return (0);
  }
  rawPressure = (dataBuffer[1] << 16) | (dataBuffer[2] << 8) | dataBuffer[3];
  rawPressure >>= 4;
  rawPressure *= ((uint32_t)100 << 2);
  rawPressure >>= 4;
  pressure = rawPressure / 100;
  return (pressure);
}

Mpl3115a2Error_enum MPL3115A2::getPressure(float &pressure) {
  uint8_t dataBuffer[4] = {0};
  uint8_t dataReady = 0;
  uint8_t status = 0;
  uint32_t rawPressure = 0;
  uint32_t timeout = millis();
  while (!(dataReady & 0x08)) {
    if ((millis() - timeout) > MPL3115A2_TIMEOUT) {
#ifdef DEBUG
      Serial.println(F("Sensor timeout!"));
#endif // DEBUG
      pressure = 0;
      return (MPL3115A2_STATUS_TIMEOUT);
    }
    status = this->readRegister(MPL3115A2_STATUS, &dataReady);
    if (status != I2C_ERROR_OK) {
#ifdef DEBUG
      Serial.println(F("I2C error!"));
#endif // DEBUG
      pressure = 0;
      return (MPL3115A2_ERROR);
    }
  }
  status = this->readRegister(MPL3115A2_STATUS, dataBuffer, sizeof(dataBuffer));
  if (status != I2C_ERROR_OK) {
#ifdef DEBUG
    Serial.println(F("I2C error!"));
#endif // DEBUG
    pressure = 0;
    return (MPL3115A2_ERROR);
  }
  rawPressure = (dataBuffer[1] << 16) | (dataBuffer[2] << 8) | dataBuffer[3];
  rawPressure >>= 4;
  rawPressure *= ((uint32_t)100 << 2);
  rawPressure >>= 4;
  pressure = rawPressure / 100;
  return (MPL3115A2_OK);
}

float MPL3115A2::getAltitude() {
  uint8_t dataBuffer[4] = {0};
  uint8_t dataReady = 0;
  uint8_t status = 0;
  uint32_t rawAltitude = 0;
  float altitude = 0;
  uint32_t timeout = millis();
  while (!(dataReady & 0x08)) {
    if ((millis() - timeout) > MPL3115A2_TIMEOUT) {
#ifdef DEBUG
      Serial.println(F("Sensor timeout!"));
#endif // DEBUG
      return (0);
    }
    status = this->readRegister(MPL3115A2_STATUS, &dataReady);
    if (status != I2C_ERROR_OK) {
#ifdef DEBUG
      Serial.println(F("I2C error!"));
#endif // DEBUG
      return (0);
    }
  }
  status = this->readRegister(MPL3115A2_STATUS, dataBuffer, sizeof(dataBuffer));
  if (status != I2C_ERROR_OK) {
#ifdef DEBUG
    Serial.println(F("I2C error!"));
#endif // DEBUG
    return (0);
  }
  rawAltitude = (dataBuffer[1] << 16) | (dataBuffer[2] << 8) | dataBuffer[3];
  rawAltitude >>= 4;
  rawAltitude *= ((uint32_t)100 << 4);
  rawAltitude >>= 8;
  altitude = rawAltitude / 100;
  return (altitude);
}

Mpl3115a2Error_enum MPL3115A2::getAltitude(float &altitude) {
  uint8_t dataBuffer[4] = {0};
  uint8_t dataReady = 0;
  uint8_t status = 0;
  uint32_t rawAltitude = 0;
  uint32_t timeout = millis();
  while (!(dataReady & 0x08)) {
    if ((millis() - timeout) > MPL3115A2_TIMEOUT) {
#ifdef DEBUG
      Serial.println(F("Sensor timeout!"));
#endif // DEBUG
      altitude = 0;
      return (MPL3115A2_STATUS_TIMEOUT);
    }
    status = this->readRegister(MPL3115A2_STATUS, &dataReady);
    if (status != I2C_ERROR_OK) {
#ifdef DEBUG
      Serial.println(F("I2C error!"));
#endif // DEBUG
      altitude = 0;
      return (MPL3115A2_ERROR);
    }
  }
  status = this->readRegister(MPL3115A2_STATUS, dataBuffer, sizeof(dataBuffer));
  if (status != I2C_ERROR_OK) {
#ifdef DEBUG
    Serial.println(F("I2C error!"));
#endif // DEBUG
    altitude = 0;
    return (MPL3115A2_ERROR);
  }
  rawAltitude = (dataBuffer[1] << 16) | (dataBuffer[2] << 8) | dataBuffer[3];
  rawAltitude >>= 4;
  rawAltitude *= ((uint32_t)100 << 4);
  rawAltitude >>= 8;
  altitude = rawAltitude / 100;
  return (MPL3115A2_OK);
}

void MPL3115A2::printStatus(Mpl3115a2Error_enum status) {
  switch (status) {
  case MPL3115A2_OK:
    Serial.println(F("MPL3115A2_OK"));
    break;
  case MPL3115A2_ERROR:
    Serial.println(F("MPL3115A2_ERROR"));
    break;
  case MPL3115A2_STATUS_TIMEOUT:
    Serial.println(F("MPL3115A2_STATUS_TIMEOUT"));
    break;
  case MPL3115A2_STATUS_NOT_IMPLEMENTED:
    Serial.println(F("MPL3115A2_STATUS_NOT_IMPLEMENTED"));
    break;
  default:
    Serial.println(F("Invalid status"));
    break;
  }
}