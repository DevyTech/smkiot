#define DO_PIN 26  // ESP32's pin GPIO26 connected to DO pin of the MQ2 sensor
#define AO_PIN 32  // ESP32's pin GPIO34 connected to AO pin of the MQ2 sensor (pin ADC1_CH6)

// Function to initialize the setup
void setup() {
  // initialize serial communication
  Serial.begin(9600);
  
  // initialize the ESP32's pin as an input for digital output
  pinMode(DO_PIN, INPUT);

  // Warm up the MQ2 sensor
  Serial.println("Warming up the MQ2 sensor");
  //delay(20000);  // wait for the MQ2 to warm up
}

// Function to handle the main loop
void loop() {
  // Read digital output from the MQ2 sensor
  int gasState = digitalRead(DO_PIN);

  // Check if gas is detected via digital output
  if (gasState == LOW) {
    Serial.println("Gas Detected");
  }

  // Read analog value from the MQ2 sensor
  int sensorValue = analogRead(AO_PIN);
  if (sensorValue >=1300 && sensorValue <= 1599){
    Serial.print("Gas Concentration: ");
  }
  Serial.println(sensorValue);

  // Delay 1 second before the next reading
  delay(1000);
}
