#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_BME280.h>
#include <Adafruit_INA260.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>
#include <SparkFun_Qwiic_OpenLog_Arduino_Library.h>
#include <TimeLib.h> // Include Time library for Teensy hardware RTC

// Initialize with the default I2C address (0x28)
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);
Adafruit_BME280 bme;
Adafruit_INA260 ina260 = Adafruit_INA260();
SFE_UBLOX_GNSS myGNSS;
OpenLog myLog;


time_t getTeensy3Time() {
  return Teensy3Clock.get();
}

void setup() {
  Serial.begin(115200);
  myLog.begin();
  

  setSyncProvider(getTeensy3Time);

  if (!bno.begin()) {
    Serial.println("No BNO055 detected... Check your wiring or I2C ADDR!");
    while (1);
  }
  delay(1000);
  bno.setExtCrystalUse(true);

  if (!bme.begin()) {
    Serial.println("Could not find a valid BME280 Sensor, check wiring!");
    while (1);
  }

  if (!ina260.begin()) {
    Serial.println("Could not find a valid ina260 Sensor, check wiring!");
    while (1);
  }

  if (!myGNSS.begin()) {
    Serial.println("u-blox GNSS not detected at default I2C address. Check wiring");
    while (1);
  }

  // Updated OpenLog header to include ISO Date and Time columns
  myLog.println("Date,Time,Timestamp_ms,Temp (*C),Humidity (%),Pressure (hPa),X Orient,Y Orient,Z Orient,Current,Bus Voltage,Power,Latitude,Longitude,Altitude");
}

void loop() {
  // Get a new sensor event
  sensors_event_t event;
  bno.getEvent(&event);

  float temp = bme.readTemperature();
  float hum = bme.readHumidity();
  float pres = bme.readPressure() / 100.0F;

  float x_orient = event.orientation.x;
  float y_orient = event.orientation.y;
  float z_orient = event.orientation.z;

  float current = ina260.readCurrent();
  float bus_voltage = ina260.readBusVoltage();
  float power = ina260.readPower();

  if (myGNSS.getPVT() == true) {
    // 1. Check if GPS time is valid. If yes, update the Teensy Hardware RTC.
    if (myGNSS.getGnssFixOk() && myGNSS.getTimeValid() && myGNSS.getDateValid()) {
      tmElements_t gpsTime;
      gpsTime.Year = myGNSS.getYear() - 1970; // TimeLib expects years since 1970
      gpsTime.Month = myGNSS.getMonth();
      gpsTime.Day = myGNSS.getDay();
      gpsTime.Hour = myGNSS.getHour();
      gpsTime.Minute = myGNSS.getMinute();
      gpsTime.Second = myGNSS.getSecond();
      

      Teensy3Clock.set(makeTime(gpsTime));
    }

    // Convert GPS integers to floats with correct scaling
    float latitude = myGNSS.getLatitude() / 10000000.0;    // Scaled 10^7 to Degrees
    float longitude = myGNSS.getLongitude() / 10000000.0;  // Scaled 10^7 to Degrees
    float altitude = myGNSS.getAltitudeMSL() / 1000.0;     // Millimeters to Meters

    // Create string buffers for formatted date & time strings
    char dateStr[12];
    char timeStr[9];
    snprintf(dateStr, sizeof(dateStr), "%04d-%02d-%02d", year(), month(), day());
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d", hour(), minute(), second());

    // Log tracking elements to OpenLog
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
    myLog.print(latitude);
    myLog.print(",");
    myLog.print(longitude);
    myLog.print(",");
    myLog.println(altitude);


    Serial.println("| Date       | Time     | Temp (*C) | Humidity (%) | Pressure (hPa) | X Orient | Y Orient | Z Orient | Current | Bus Voltage | Power | latitude | longitude | altitude |");
    Serial.println("|------------|----------|-----------|--------------|----------------|----------|----------|----------|---------|-------------|-------|----------|-----------|----------|");

    Serial.printf("| %s | %s | %-9.2f | %-12.1f | %-14.2f | %-8.2f | %-8.2f | %-8.2f | %-7.2f | %-11.2f | %-5.2f | %-8.5f | %-9.5f | %-8.2f |\n",
                  dateStr, timeStr, temp, hum, pres, x_orient, y_orient, z_orient, current, bus_voltage, power, latitude, longitude, altitude);

    Serial.println("|------------|----------|-----------|--------------|----------------|----------|----------|----------|---------|-------------|-------|----------|-----------|----------|");
  }
  delay(25);
}
