#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_BME280.h>
#include <Adafruit_INA260.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>
#include <SparkFun_Qwiic_OpenLog_Arduino_Library.h>
#include <TimeLib.h>  // Include Time library for Teensy hardware RTC

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
  // 1. Check for new GPS data. 
  // If you used myGNSS.setAutoPVT(true) in setup(), this check is instant!
  if (myGNSS.getPVT() == true) {
    
    // =================================================================
    //  LOW-FREQUENCY TASK: Update the RTC (Runs once per minute)
    // =================================================================
    static unsigned long lastRtcSync = 0;
    const unsigned long SYNC_INTERVAL = 60000; // 60,000 ms = 1 minute

    if ((millis() - lastRtcSync >= SYNC_INTERVAL) || lastRtcSync == 0) {
      // Ensure we have a valid 3D fix and valid time/date from satellites
      if (myGNSS.getGnssFixOk() && myGNSS.getTimeValid() && myGNSS.getDateValid()) {
        
        uint32_t epoch = myGNSS.getUnixEpoch(); // Fetch Unix time directly
        
        Teensy3Clock.set(epoch); // Sync Hardware RTC
        setTime(epoch);          // Sync TimeLib Software Time
        
        lastRtcSync = millis();  // Reset the timer
      }
    }


    // =================================================================
    //  HIGH-FREQUENCY TASKS: Read Navigation Data (Runs 1-10+ times/sec)
    // =================================================================
    // These functions simply pull variables from the microcontroller's RAM. 
    // They have practically zero overhead.
    
    long latitude = myGNSS.getLatitude();
    long longitude = myGNSS.getLongitude();
    long altitude = myGNSS.getAltitude();
    long speed = myGNSS.getGroundSpeed();
    byte satellites = myGNSS.getSIV(); // Satellites In View
    
    // You can process, print, or log this navigation data here
    /*
    Serial.print("Lat: ");
    Serial.print(latitude / 10000000.0, 7);
    Serial.print(" | Speed: ");
    Serial.println(speed);
    */
  }


  // =================================================================
  //  OTHER NON-BLOCKING SYSTEM TASKS
  // =================================================================
  // Because your GPS code no longer blocks or wastes time on heavy math,
  // the rest of your loop can run hundreds of thousands of times a second.
  
  // checkButtons();
  // readIMU();
  // updateOLEDDisplay();
  // writeToSDCard();
}
