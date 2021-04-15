#include "version.hpp"

#define SERIAL_BAUD 115200

void setup() {

  //Init serial connection
  Serial.begin(SERIAL_BAUD);
  Serial.setTimeout(1);

  Serial.print(BUILD_PROGRAM);
  Serial.print(":");
  Serial.print(BUILD_DATE);
  Serial.print("-");
  Serial.print(BUILD_VERSION);
  Serial.print("\n");
  Serial.flush();
  Serial.print("\r\n");

  Serial.print("Serial baud used: ");
  Serial.print(SERIAL_BAUD);
  Serial.print("\r\n");
  Serial.print("\r\n-------------------------------------------\r\n");
  Serial.print("\r\n");
}

void loop() {
  if ( Serial.available() ) {
    byte type = Serial.read();
    Serial.print("read: ");
    Serial.write(type);
    Serial.print("(");
    Serial.print(type);
    Serial.print(")\r\n");
  }
}
