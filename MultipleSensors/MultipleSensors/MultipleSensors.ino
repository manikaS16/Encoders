/**
 * Author: @Manika
 * Hardware: This program runs on an Arduino. It talks to a magnetic sensor called the AS5600. The TCA9548A mux selects the sensor in use
 * Input: AS5600 sensor measures the angle of a rotating magnet
 * Processing: The TCA9548A selects the sensor via select lines and the sensor sends this angle to the Arduino
 * Output: Arduino shows it on your computer screen in degrees (0° to 360°)
 */

#include <Wire.h> // Library which help arduino to communicate with other devices using I2C

#define TCA9548A_ADDR 0x70  // Default address of the multiplexer
#define AS5600_ADDRESS 0x36  // Address of the AS5600 sensor on I2C bus
#define ANGLE_MSB_REGISTER 0x0E //  Specific spot inside the sensor which holds the angle information
#define MAX_SENSORS 4 // Maximum number of sensors we have connected to

/******************************* Helper functions ****************************/

/**
 * Input: sensor number you want to activate using mux
 * Processing: The TCA9548A multiplexer switches its internal connection to allow communication only with the sensor connected to the specified channel
 */
void selectMuxChannel(uint8_t sensor) {
  if (sensor > 7) return; // Safety check: TCA9548A supports channels 0–7 only
  
  Wire.beginTransmission(TCA9548A_ADDR); // Arduino starts taking to TCA9548A multiplexer
  Wire.write(1 << sensor);  // Bitmask: sensor 0 = 00000001, 1 = 00000010, etc. The TCA9548A multiplexer uses a single byte where each bit represents a channel, To activate one specific channel, you set only that bit to 1
  Wire.endTransmission();
}

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
  Wire.begin();       // Initialize I2C communication
  Serial.begin(9600); // Start serial communication at 9600 baud
}

void loop() {
  for (uint8_t sensor = 0; sensor < MAX_SENSORS; sensor++) { // For each sensor
    selectMuxChannel(sensor); // Select the sensor via mux

    uint16_t rawAngle = readRawAngle();
    float angleInDegrees = rawAngle * (360.0f / 4096.0f); // Convert to degrees (0 - 360° range)

    Serial.print("Sensor ");
    Serial.print(sensor);
    Serial.print(": ");
    Serial.print("Angle: ");
    Serial.print(angleInDegrees, 2);
    Serial.println("°");

    delay(200); // Wait 200ms before next reading
  }

  Serial.println();
  delay(500); // Delay before repeating all sensors
}
