// Change the target port to Serial4 (Pin 17 for TX, Pin 16 for RX)
#define OpenLogSerial Serial4

void setup() {
  Serial.begin(115200);       // USB Serial to Computer Monitor
  OpenLogSerial.begin(9600);  // UART Serial to OpenLog (Factory default speed)

  delay(2000);  // Give the OpenLog card time to spin up

  Serial.println("Teensy 4.0 Online. Streaming data out of Pin 17 (TX4)...");
  OpenLogSerial.println("--- Telemetry Log Started on Serial4 ---");
}

void loop() {
  Serial.println("Logged data...");  // Verifies loop speed on your computer screen

  static int recordCount = 0;
  float simulatedSensorVal = analogRead(A0) * (3.3 / 1023.0);

  // Stream the data out of Pin 17
  OpenLogSerial.print("Count: ");
  OpenLogSerial.print(recordCount++);
  OpenLogSerial.print(" | Volts: ");
  OpenLogSerial.println(simulatedSensorVal);

  delay(1000);
}
