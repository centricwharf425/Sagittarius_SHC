#include <wiring.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_BME280.h>
#include <Adafruit_INA260.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>
#include <TimeLib.h>
#include "utils.h"

// Define myLog and link it to your actual OpenLog Hardware Serial Port
HardwareSerial& myLog = Serial4;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 4000);  // Wait up to 4 seconds for screen connection
  Serial.println("Code has begun Initialization");

  myLog.begin(9600);  // UART Serial to OpenLog

  Wire.begin();
  Wire.setClock(400000);  // Set the speed of the I2C Communication

  sensor_validation();
  setSyncProvider(getTeensy3Time);

  Serial.println("Setup complete successfuully. Waiting for GPS PVT packets...");
}

void loop() {
  unsigned long currentMillis = millis();

  sensors_event_t event;
  bno.getEvent(&event);

  // FIXED: Removed 'float' to use the global extern variables
  temp = bme.readTemperature();
  hum = bme.readHumidity();
  pres = bme.readPressure();

  x_orient = event.orientation.x;
  y_orient = event.orientation.y;
  z_orient = event.orientation.z;

  current = ina260.readCurrent();
  bus_voltage = ina260.readBusVoltage();
  power = ina260.readPower();

  if (currentMillis - previousMillis >= LOG_PERIOD) {
    previousMillis = currentMillis;

    cpmSnapshot = counts;
    counts = 0;
  }

  if (myGNSS.getPVT(2000) == true) {
    if (myGNSS.getGnssFixOk() && myGNSS.getTimeValid() && myGNSS.getDateValid()) {
      tmElements_t gpsTime;
      gpsTime.Year = myGNSS.getYear() - 1970;
      gpsTime.Month = myGNSS.getMonth();
      gpsTime.Day = myGNSS.getDay();
      gpsTime.Hour = myGNSS.getHour();
      gpsTime.Minute = myGNSS.getMinute();
      gpsTime.Second = myGNSS.getSecond();

      Teensy3Clock.set(makeTime(gpsTime));
    }

    // FIXED: Removed 'float' to save data where log_all() can see it
    latitude = myGNSS.getLatitude();
    longitude = myGNSS.getLongitude();
    altitude = myGNSS.getAltitudeMSL();

    // FIXED: Removed 'char' array declarations to use global string buffers
    snprintf(dateStr, sizeof(dateStr), "%04d-%02d-%02d", year(), month(), day());
    snprintf(timeStr, sizeof(timeStr), "%02d-%02d-%02d", hour(), minute(), second());

    // Print Telemetry Visual Table to standard serial monitor
    Serial.println("\n| Date       | Time     | Runtime_ms | Temp (*C) | Humidity (%) | Pressure (hPa) | X Orient | Y Orient | Z Orient | Current | Bus Voltage | Power | latitude | longitude | altitude | CPM   |");
    Serial.println("|------------|----------|------------|-----------|--------------|----------------|----------|----------|----------|---------|-------------|-------|----------|-----------|----------|-------|");

    // 16 layout specifiers matching exactly 16 passed variables
    Serial.printf("| %s | %s | %-10lu | %-9.2f | %-12.1f | %-14.2f | %-8.2f | %-8.2f | %-8.2f | %-7.2f | %-11.2f | %-5.2f | %-8.5f | %-9.5f | %-8.2f | %-5lu |\n",
                  dateStr, timeStr, millis(), temp, hum, pres, x_orient, y_orient, z_orient, current, bus_voltage, power, latitude, longitude, altitude, cpmSnapshot);

    Serial.println("|------------|----------|------------|-----------|--------------|----------------|----------|----------|----------|---------|-------------|-------|----------|-----------|----------|-------|");

    // EXECUTE TELETEMRY LOGGING TO OPENLOG SD CARD
    log_all();
  } else {
    Serial.println("Get PVT False...");
  }
}
