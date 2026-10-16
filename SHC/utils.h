#pragma once
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_BME280.h>
#include <Adafruit_INA260.h>
#include "SparkFun_u-blox_GNSS_Arduino_Library.h"
#include <TimeLib.h> // Added dependency for time_t definition

extern Adafruit_BNO055 bno;
extern Adafruit_BME280 bme;
extern Adafruit_INA260 ina260;
extern SFE_UBLOX_GNSS myGNSS;

extern const int geigerPin;
extern volatile unsigned long counts;
extern unsigned long previousMillis;
extern unsigned long cpmSnapshot; // FIXED: Duplication error removed from lower lines
extern unsigned long lastheartbeat;

extern HardwareSerial& myLog;

extern char dateStr[12];
extern char timeStr[9];

extern float temp;
extern float hum;
extern float pres;

extern float x_orient;
extern float y_orient;
extern float z_orient;

extern float current;
extern float bus_voltage;
extern float power;

extern float latitude;
extern float longitude;
extern float altitude;

#define LOG_PERIOD 60000

time_t getTeensy3Time();
void sensor_validation();
void impulse();
void log_all();
