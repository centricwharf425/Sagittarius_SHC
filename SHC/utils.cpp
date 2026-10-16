#include "core_pins.h"
#include "utils.h"

// Initialize with the default I2C address (0x28)
Adafruit_BNO055 bno;
Adafruit_BME280 bme;
Adafruit_INA260 ina260;
SFE_UBLOX_GNSS myGNSS;

// Initialize Variables
const int geigerPin = 4;
volatile unsigned long counts = 0;
unsigned long previousMillis = 0;
unsigned long cpmSnapshot = 0;
unsigned long lastheartbeat = 0;

char dateStr[12] = "0000-00-00";
char timeStr[9] = "00-00-00";

float temp = 0.0;
float hum = 0.0;
float pres = 0.0;

float x_orient = 0.0;
float y_orient = 0.0;
float z_orient = 0.0;

float current = 0.0;
float bus_voltage = 0.0;
float power = 0.0;

float latitude = 0.0;
float longitude = 0.0;
float altitude = 0.0;

void impulse() {
  counts++;
}

time_t getTeensy3Time() {
  return Teensy3Clock.get();
}

void log_all() {
  // Log clean comma-separated values to OpenLog
  myLog.print(dateStr);
  myLog.print(",");
  myLog.print(timeStr);
  myLog.print(",");
  myLog.print(millis());
  myLog.print(",");
  myLog.print(temp);
  myLog.print(",");
  myLog.print(hum);
  myLog.print(",");
  myLog.print(pres);
  myLog.print(",");
  myLog.print(x_orient);
  myLog.print(",");
  myLog.print(y_orient);
  myLog.print(",");
  myLog.print(z_orient);
  myLog.print(",");
  myLog.print(current);
  myLog.print(",");
  myLog.print(bus_voltage);
  myLog.print(",");
  myLog.print(power);
  myLog.print(",");
  myLog.print(latitude, 7);
  myLog.print(",");  // 7 decimals is ~1 cm resolution
  myLog.print(longitude, 7);
  myLog.print(",");
  myLog.print(altitude);
  myLog.print(",");
  myLog.println(cpmSnapshot);  // Line ending creates a clean carriage return row
}

void sensor_validation() {
  // Check the bno if it is connected
  if (!bno.begin()) {
    Serial.println("No BNO055 detected...");
    while (1);
  }

  if (!bme.begin()) {
    Serial.println("No BME280 detected...");
    while (1);
  }

  if (!ina260.begin()) {
    Serial.println("No INA260 detected...");
    while (1);
  }
  
  if (!myGNSS.begin()) {
    Serial.println("No u-blox GNSS detected..."); // FIXED typo string
    while (1);
  }

  pinMode(geigerPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(geigerPin), impulse, FALLING);
  Serial.println("All Sensors have been Initialized.");
}
