#include <Arduino.h>
#include <MPL3115A2.h>
#include <Wire.h>

const static int I2C_BUS = 0;
const static int I2C_SCL = 21;
const static int I2C_SDA = 22;
const static int I2C_SPEED = 100000;

TwoWire *i2c;
MPL3115A2 sensor;

void initI2C();

void setup() {
  Serial.begin(115200);
  initI2C();
  sensor.init(i2c);
  sensor.setOversamplingRatio(MPL3115A2_OS_512);
}

void loop() {
  sensor.setModeBarometer();
  delay(MPL3115A2_OS_512_DELAY);
  float pressure;
  sensor.getPressure(pressure);
  sensor.setModeAltimeter();
  delay(MPL3115A2_OS_512_DELAY);
  float altitude;
  sensor.getAltitude(altitude);
  Serial.print(millis());
  Serial.print(F(":\t"));
  Serial.print(F("Pressure (Pa):\t"));
  Serial.print(pressure);
  Serial.print(F("\t"));
  Serial.print(F("Altitude (m):\t"));
  Serial.println(altitude);
}

void initI2C() {
  i2c = new TwoWire(I2C_BUS);
  i2c->begin(I2C_SDA, I2C_SCL, 100000);
}