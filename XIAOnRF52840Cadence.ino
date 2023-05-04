#include <LSM6DS3.h>
#include <Wire.h>


LSM6DS3 myIMU(I2C_MODE, 0x6A);
float aX, aY, aZ, gX, gY, gZ;
const float accelerationThreshold = 2.5; // threshold of significant in G's
const int numSamples = 119;
int samplesRead = numSamples;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  while (myIMU.begin() != 0) {
    Serial.println("Device error");
    delay(1000);
  }

  // using serial plotter
  Serial.print("GYRO_X");
  Serial.print(",");
  Serial.print("GYRO_Y");
  Serial.print(",");
  Serial.println("GYRO_Z");

}

void loop() {

  Serial.print(myIMU.readFloatGyroX(), 3);
  Serial.print(',');
  Serial.print(myIMU.readFloatGyroY(), 3);
  Serial.print(',');
  Serial.print(myIMU.readFloatGyroZ(), 3);
  Serial.println();

  delay(500);

}