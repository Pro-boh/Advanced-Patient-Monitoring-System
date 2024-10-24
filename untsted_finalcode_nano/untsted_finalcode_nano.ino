#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Display
Adafruit_SSD1306 cham = Adafruit_SSD1306(128, 64, &Wire);

// LM35 Temperature Sensor
int lm35Pin = A1;  // LM35 connected to analog pin A1
float temperature = 0;

// AD8232 ECG Sensor
int ecgPin1 = 10;  // ECG sensor L0+ connected to pin 10
int ecgPin2 = 11;  // ECG sensor L0- connected to pin 11
int ecgValue;

// IR Sensors
const int irSensor1Pin = 2;  // Pin for IR sensor 1 output

// HW-828 Pulse Sensor
int pulsePin = A0;            // Pulse Sensor purple wire connected to analog pin A0
int blinkPin = 13;            // Pin to blink LED at each beat

// Button
const int buttonPin = 7;      // Pin for the button
bool buttonPressed = false;   // Track button state
int value = 107;              // Start value for decreasing

// Volatile Variables for Pulse Sensor
volatile int BPM;                   // Beats per minute
volatile int Signal;                // Raw data from pulse sensor
volatile int IBI = 600;             // Time interval between beats
volatile boolean Pulse = false;     // True when heartbeat is detected
volatile boolean QS = false;        // Becomes true when a beat is found

volatile int rate[10];              // Array to hold the last 10 IBI values
volatile unsigned long sampleCounter = 0;
volatile unsigned long lastBeatTime = 0;
volatile int P = 512;
volatile int T = 512;
volatile int thresh = 525;
volatile int amp = 100;
volatile boolean firstBeat = true;
volatile boolean secondBeat = false;

// SoftwareSerial for UART communication
SoftwareSerial espSerial(3, 4); // RX, TX (Pin 3 as RX and Pin 4 as TX)

void setup() {
  Serial.begin(115200);        // Initialize serial communication
  espSerial.begin(9600);       // Initialize SoftwareSerial for communication to ESP32

  // OLED
  cham.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // Initialize ECG Sensor Pins
  pinMode(ecgPin1, INPUT);   // Set ECG pin 1 as input
  pinMode(ecgPin2, INPUT);   // Set ECG pin 2 as input

  // Initialize IR Sensor Pins
  pinMode(irSensor1Pin, INPUT); // Set IR sensor 1 pin as input

  // Initialize Pulse Sensor Pin
  pinMode(blinkPin, OUTPUT);  // Set blink pin as output for heartbeat detection

  // Initialize Button
  pinMode(buttonPin, INPUT_PULLUP); // Set button pin as input with internal pull-up

  interruptSetup();  // Set up pulse sensor interrupt
}

void loop() {
  // OLED
  cham.clearDisplay();
  cham.setTextColor(WHITE);
  cham.setTextSize(2);

  // Check button state
  if (digitalRead(buttonPin) == LOW) { // Button pressed
    buttonPressed = !buttonPressed;    // Toggle the button state
    delay(300);                        // Debounce delay
  }

  // LM35 Temperature Sensor
  int tempValue = analogRead(lm35Pin); // Read the analog input from LM35
  temperature = tempValue * (5.0 / 1023.0) * 100.0; // Convert to Celsius

  // AD8232 ECG Sensor
  if ((digitalRead(ecgPin1) == 1) || (digitalRead(ecgPin2) == 1)) {
    ecgValue = -1; // Indicate error or issue with ECG
  } else {
    ecgValue = analogRead(A0); // Read ECG value from analog input
  }

  // IR Sensors
  int sensorValue1 = digitalRead(irSensor1Pin); // Read value from IR sensor 1

  // Output Temperature, ECG, IR sensor values, and BPM in one line
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, ECG: ");

  // Output Temp, ECG, IR in 3 rows on OLED
  cham.setCursor(0, 0);
  cham.print("Temp:");
  cham.print(temperature);
  cham.print(" C");
  
  if (ecgValue == -1) {
    Serial.print("Error");
    cham.setCursor(0, 15);
    cham.print("ECG:");
    cham.print("Error");
  } else {
    Serial.print(ecgValue);
    cham.setCursor(0, 15);
    cham.print("ECG:");
    cham.print(ecgValue);
  }

  // Perform the calculation if buttonPressed is true
  if (buttonPressed && sensorValue1 == 1) {
    value -= 2; // Decrease by 2
    if (value < 0) value = 0; // Prevent value from going negative
  }

  // Replace IR display with remaining value display
  cham.setCursor(0, 30);
  cham.print("Waist:");
  cham.print(value);
  cham.print(" cm");

  cham.display(); // Display the updated values

  delay(1000); // Wait for 1 second before next reading
}

// Pulse Sensor interrupt setup function
void interruptSetup() {     
  TCCR2A = 0x02; // Disable PWM on pins 3 and 11, enter CTC mode
  TCCR2B = 0x06; // 256 prescaler
  OCR2A = 0X7C;  // Set top of count to 124 for 500Hz sample rate
  TIMSK2 = 0x02; // Enable interrupt on match between Timer2 and OCR2A
  sei();         // Ensure global interrupts are enabled      
}

// Function to handle output when beat happens
void serialOutputWhenBeatHappens() {
  Serial.print(" Heart-Beat Found ");
  Serial.print("BPM: ");
  Serial.println(BPM);

  cham.setCursor(0, 45);
  cham.print("BPM: ");
  cham.print(BPM);
  delay(1000);
}

ISR(TIMER2_COMPA_vect) {
  cli();  
  Signal = analogRead(pulsePin); 
  sampleCounter += 2; 
  int N = sampleCounter - lastBeatTime; 
  
  if (Signal < thresh && N > (IBI/5)*3) {
    if (Signal < T) {
      T = Signal; 
    }
  }

  if (Signal > thresh && Signal > P) {
    P = Signal; 
  }

  if (N > 250) {
    if ((Signal > thresh) && (Pulse == false) && (N > (IBI/5)*3)) {
      Pulse = true;  
      digitalWrite(blinkPin, HIGH); 
      IBI = sampleCounter - lastBeatTime; 
      lastBeatTime = sampleCounter;
      
      if (secondBeat) {
        secondBeat = false;
        for (int i = 0; i <= 9; i++) {
          rate[i] = IBI;
        }
      }

      if (firstBeat) {
        firstBeat = false;
        secondBeat = true;
        sei();
        return;
      }

      word runningTotal = 0;
      for (int i = 0; i <= 8; i++) {
        rate[i] = rate[i+1];
        runningTotal += rate[i];
      }

      rate[9] = IBI;
      runningTotal += rate[9];
      runningTotal /= 10;
      BPM = 60000 / runningTotal;
      QS = true;  
    }
  }

  if (Signal < thresh && Pulse == true) {
    digitalWrite(blinkPin, LOW); 
    Pulse = false;
    amp = P - T;
    thresh = amp / 2 + T;
    P = thresh;
    T = thresh;
  }

  if (N > 2500) {
    thresh = 512;
    P = 512;
    T = 512;
    lastBeatTime = sampleCounter;
    firstBeat = true;
    secondBeat = false;
  }

  sei(); 
}
