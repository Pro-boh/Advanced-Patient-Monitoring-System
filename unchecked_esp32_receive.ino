void setup() {
  Serial.begin(115200);   // Initialize Serial communication
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // Initialize UART on pins 16 (RX) and 17 (TX)
}

void loop() {
  if (Serial2.available()) {
    String receivedData = Serial2.readStringUntil('\n'); // Read data until newline
    String temperature = receivedData.substring(0, receivedData.indexOf(',')); // Extract temperature
    receivedData = receivedData.substring(receivedData.indexOf(',') + 1);
    String ecgValue = receivedData.substring(0, receivedData.indexOf(',')); // Extract ECG value
    receivedData = receivedData.substring(receivedData.indexOf(',') + 1);
    String sensorValue1 = receivedData.substring(0, receivedData.indexOf(',')); // Extract IR sensor value
    String BPM = receivedData.substring(receivedData.indexOf(',') + 1); // Extract BPM value
    
    // Print them separately
    Serial.print("Temperature: ");
    Serial.println(temperature);
    Serial.print("ECG Value: ");
    Serial.println(ecgValue);
    Serial.print("IR Sensor 1: ");
    Serial.println(sensorValue1);
    Serial.print("BPM: ");
    Serial.println(BPM);
  }
}
