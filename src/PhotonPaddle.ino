/*
 * Project PhotonPaddle
 * Description: Firmware for the Photon Paddle, an IoT ping-pong paddle!
 * Author: berlinquin
 * Date: 8/17/17
 */

#include <adafruit-led-backpack.h>
#include <LIS3DH.h>

#include "Particle.h"

Adafruit_7segment matrix = Adafruit_7segment();

LIS3DHSPI accel(SPI, A2, WKP);

// Accelerometer values
// Print 1 samples per second to serial
const unsigned long PRINT_SAMPLE_PERIOD = 1000;
const int MAX_READING = 32768;
const int RANGE = 2;
const float G = 9.81;
unsigned long lastPrintSample = 0;

void setup() {
  pinMode(D2, INPUT_PULLDOWN);
  pinMode(D7, OUTPUT);

  // Setup Accelerometer
  LIS3DHConfig config;
  config.setAccelMode(LIS3DH::RATE_100_HZ);
  bool setupSuccess = accel.setup(config);
  bool calibrateSuccess = accel.calibrateFilter(3000);

  // Setup LED Matrix
  matrix.begin(0x70);
  matrix.writeDisplay();

  // Start serial connection
  Serial.begin(9600);
}

void loop() {
  // Decide if Accelerometer should be sampled
  if (millis() - lastPrintSample >= PRINT_SAMPLE_PERIOD) {
  lastPrintSample = millis();

  LIS3DHSample sample;
  if (accel.getSample(sample)) {
      double x, y, z;
      x = analyze_raw_data(sample.x);
      y = analyze_raw_data(sample.y);
      z = analyze_raw_data(sample.z);
      String strX, strY, strZ;
      strX = String(x, 3);
      strY = String(y, 3);
      strZ = String(z, 3);

          // Publish acceleration data
    String message = "X Acceleration: "+strX+"; Y Acceleration: "+strY+"; Z Acceleration: "+strZ;
    Particle.publish("Acceleration Data", message);

    //Print Z-acceleration to matrix
    matrix.printFloat(z);
    matrix.writeDisplay();

    // Print data to serial
    Serial.println(message);

    delay(1000);
  }
  }
}

double analyze_raw_data(int x) {
    double data = (double) x;
    data /= MAX_READING;
    data *= RANGE;
    return data * G;
}
