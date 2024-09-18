#define RAIN_SENSOR_PIN 34  // Pin D25
#define BUZZER_PIN 15       // Pin D15 untuk buzzer

void setup() {
  Serial.begin(9600); // Memulai komunikasi serial pada baud rate 115200
  pinMode(RAIN_SENSOR_PIN, INPUT);  // Menetapkan pin D4 sebagai input
  pinMode(BUZZER_PIN, OUTPUT); //setup pin buzzer menjadi output
  digitalWrite(BUZZER_PIN, LOW);    // Mematikan buzzer saat mulai
}

void loop() {
  delay(10);
  int rain_state = digitalRead(RAIN_SENSOR_PIN);  // Membaca nilai dari sensor hujan

  if (rain_state == HIGH) {  // Jika tidak ada hujan
    Serial.println("Tidak ada hujan.");
     digitalWrite(BUZZER_PIN, HIGH);  // Menyalakan buzzer
  } else {  // Jika ada hujan
    Serial.println("Hujan terdeteksi!");
    digitalWrite(BUZZER_PIN, LOW);  // Mematikan buzzer
  }
  
  delay(1000);  // Menunggu 1 detik sebelum membaca lagi
}
