/**
 * Author: @Manika
 * Hardware: This program runs on an Arduino. It talks to a magnetic sensor called the AS5600
 * Input: AS5600 sensor measures the angle of a rotating magnet
 * Processing: The sensor sends this angle to the Arduino
 * Output: Arduino shows it on your computer screen in degrees (0° to 360°)
 * 
 * I2C: A way(protocol) of sending data between chips using only two wires
 */

#include <Wire.h> // Library which help arduino to communicate with other devices using I2C
#define AS5600_ADDRESS 0x36 // Address of the AS5600 sensor on I2C bus
#define ANGLE_MSB_REGISTER 0x0E //  Specific spot inside the sensor which holds the angle information

/******************************* Helper functions ****************************/

/**
 * Input: Void
 * Processing: Reads the raw 12-bit angle from the AS5600 sensor
 * Output: A value between 0 and 4095 representing the angular position
 */
uint16_t readRawAngle() {
  Wire.beginTransmission(AS5600_ADDRESS); // Arduino starts taking to AS5600 sensor
  Wire.write(ANGLE_MSB_REGISTER);  // Arduino asks the sensor to send the angle data
  Wire.endTransmission();

  Wire.requestFrom(AS5600_ADDRESS, 2);  // Requests 2 bytes from the sensor: MSB (0x0E) and LSB (0x0F)

  if (Wire.available() < 2) { // If data is not ready
    return 0;  
  }

  byte msb = Wire.read();  // Read MSB
  byte lsb = Wire.read();   // Read LSB

  return ((msb & 0x0F) << 8) | lsb;  // Combine into 12-bit angle
}

/***************************** Main functions ***************************/

void setup() {
  Wire.begin();              // Initialize I2C communication
  Serial.begin(9600);        // Start serial communication at 9600 baud
}

void loop() {
  uint16_t rawAngle = readRawAngle();

  float angleInDegrees = rawAngle * (360.0f / 4096.0f); // Convert to degrees (0 - 360° range)

  Serial.print("Angle: ");
  Serial.print(angleInDegrees, 2);
  Serial.println("°");

  delay(200);  // Wait 500ms before next reading
}
