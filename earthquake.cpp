#include <Wire.h>
#include <MPU6050.h>
#include <SoftwareSerial.h>
#include <RTClib.h>

// Initialize MPU6050 and RTC DS3231
MPU6050 mpu;
RTC_DS3231 rtc;

// Define pins
int speakerPin = 9; // Pin for speaker output
SoftwareSerial espSerial(10, 11); // RX, TX for ESP32 communication

// Threshold for P-waves
const float P_WAVE_THRESHOLD = 4.5;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  espSerial.begin(9600);

  // Initialize MPU6050
  if (!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G)) {
    Serial.println("MPU6050 not found!");
    while (1);
  }

  // Initialize DS3231 RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Adjust to the current time
  }

  mpu.setThreshold(3); // Set motion detection threshold
  pinMode(speakerPin, OUTPUT); // Set speaker as output
}

void loop() {
  Vector rawAccel = mpu.readRawAccel();
  float totalAcceleration = sqrt(sq(rawAccel.XAxis) + sq(rawAccel.YAxis) + sq(rawAccel.ZAxis)) / 16384.0;

  // If acceleration exceeds P-wave threshold, send alert
  if (totalAcceleration > P_WAVE_THRESHOLD) {
    Serial.println("P-wave detected!");

    // Send alert to ESP32 for email/SMS
    espSerial.println("ALERT: P-wave detected!");

    // Play speaker alert
    tone(speakerPin, 1000); // Play sound
    delay(5000); // Play for 5 seconds
    noTone(speakerPin); // Stop sound
  }

  delay(1000); // Check every second
}