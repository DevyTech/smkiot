int BUZZER_PIN = 33; // deklarasi pin buzzer

void setup() {
  Serial.begin(9600);
  // setup pin buzzer menjadi outputs
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, HIGH); //Matikan Buzzer
}

void loop() {
  // Buzzer ON
  Serial.println("ON");
  digitalWrite(BUZZER_PIN, LOW);
  delay(1000); // delay 1 detik

  // Buzzer OFF
  Serial.println("OFF");
  digitalWrite(BUZZER_PIN, HIGH);
  delay(1000); // delay 1 detik
}
