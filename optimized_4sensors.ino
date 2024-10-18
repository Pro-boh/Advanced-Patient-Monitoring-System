// LM35 Temperature Sensor
const byte lm35Pin = A1;  // LM35 connected to analog pin A1
int temperature = 0;

// AD8232 ECG Sensor
const byte ecgPin1 = 10;  // ECG sensor L0+ connected to pin 10
const byte ecgPin2 = 11;  // ECG sensor L0- connected to pin 11
int ecgValue;

// IR Sensors
const byte irSensor1Pin = 2;  // Pin for IR sensor 1 output
const byte irSensor2Pin = 3;  // Pin for IR sensor 2 output

// HW-828 Pulse Sensor
const byte pulsePin = A0;     // Pulse Sensor purple wire connected to analog pin A0
const byte blinkPin = 13;     // Pin to blink LED at each beat

// Volatile Variables for Pulse Sensor
volatile int BPM = 0;
volatile int Signal = 0;
volatile int IBI = 600;
volatile boolean Pulse = false;
volatile boolean QS = false;

volatile byte rate[10];       // Array to hold last 10 IBI values
volatile unsigned long sampleCounter = 0;
volatile unsigned long lastBeatTime = 0;
volatile int P = 512;
volatile int T = 512;
volatile int thresh = 525;
volatile int amp = 100;
volatile boolean firstBeat = true;
volatile boolean secondBeat = false;

void setup() {
  Serial.begin(115200);          // Initialize serial communication

  pinMode(ecgPin1, INPUT);       // ECG sensor pin 1
  pinMode(ecgPin2, INPUT);       // ECG sensor pin 2
  pinMode(irSensor1Pin, INPUT);  // IR sensor 1 pin
  pinMode(irSensor2Pin, INPUT);  // IR sensor 2 pin
  pinMode(blinkPin, OUTPUT);     // Pulse LED blink pin

  interruptSetup();              // Set up pulse sensor interrupt
}

void loop() {
  int tempValue = analogRead(lm35Pin); // Read LM35 temperature
  temperature = (tempValue * 5 / 1023) * 100;  // Integer calculation for temperature

  if ((digitalRead(ecgPin1) == HIGH) || (digitalRead(ecgPin2) == HIGH)) {
    ecgValue = -1;  // ECG sensor error
  } else {
    ecgValue = analogRead(lm35Pin); // Read ECG value
  }

  int sensorValue1 = digitalRead(irSensor1Pin); // Read IR sensor 1
  int sensorValue2 = digitalRead(irSensor2Pin); // Read IR sensor 2

  // Output data to serial
  Serial.print("Temp: "); Serial.print(temperature); Serial.print("C, ");
  Serial.print("ECG: ");
  if (ecgValue == -1) Serial.print("Error");
  else Serial.print(ecgValue);
  
  Serial.print(", IR1: "); Serial.print(sensorValue1);
  Serial.print(", IR2: "); Serial.print(sensorValue2);
  Serial.print(", BPM: "); Serial.println(BPM);

  if (QS == true) {
    QS = false; // Reset Quantified Self flag
  }

  delay(1000); // Delay between readings
}

// Pulse Sensor interrupt setup function
void interruptSetup() {     
  TCCR2A = 0x02; // Disable PWM on pins 3 and 11, CTC mode
  TCCR2B = 0x06; // 256 prescaler
  OCR2A = 0X7C;  // Set top of count to 124 for 500Hz sample rate
  TIMSK2 = 0x02; // Enable interrupt on match between Timer2 and OCR2A
  sei();         // Enable global interrupts      
}

ISR(TIMER2_COMPA_vect) {
  cli();  
  Signal = analogRead(pulsePin);  // Read pulse sensor
  sampleCounter += 2;             // Increment time counter
  int N = sampleCounter - lastBeatTime;

  if (Signal < thresh && N > (IBI / 5) * 3) {
    if (Signal < T) T = Signal; // Track trough
  }

  if (Signal > thresh && Signal > P) {
    P = Signal;  // Track peak
  }

  if (N > 250 && (Signal > thresh) && !Pulse && (N > (IBI / 5) * 3)) {
    Pulse = true;
    digitalWrite(blinkPin, HIGH);  // Turn on LED
    IBI = sampleCounter - lastBeatTime;
    lastBeatTime = sampleCounter;

    if (secondBeat) {
      secondBeat = false;
      for (byte i = 0; i < 10; i++) rate[i] = IBI;
    }

    if (firstBeat) {
      firstBeat = false;
      secondBeat = true;
      sei();
      return;
    }

    unsigned long runningTotal = 0;
    for (byte i = 0; i < 9; i++) {
      rate[i] = rate[i + 1];
      runningTotal += rate[i];
    }

    rate[9] = IBI;
    runningTotal += rate[9];
    runningTotal /= 10;
    BPM = 60000 / runningTotal;
    QS = true;
  }

  if (Signal < thresh && Pulse == true) {
    digitalWrite(blinkPin, LOW); // Turn off LED
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
