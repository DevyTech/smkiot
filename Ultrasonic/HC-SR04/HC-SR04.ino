#define BLYNK_TEMPLATE_ID "TMPL6pUAiNpOi"
#define BLYNK_TEMPLATE_NAME "Smart Home"
#define BLYNK_AUTH_TOKEN "uhY1Cma3_6hFso6kWMFb_wokQ8Sk3CL3"
#include <DHT.h>
#include <DHT_U.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Your Wi-Fi credentials
char ssid[] = "LABRPS";
char pass[] = "Komputer0000";

// Definisikan pin yang digunakan untuk trigger dan echo
const int trigPin = 13;
const int echoPin = 12;

// Variabel untuk menyimpan waktu perjalanan gelombang ultrasonik
long duration;

void checkWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected, attempting to connect...");
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
    }
    Serial.println("\nWiFi connected.");
  }
}

void setup() {
  // Mulai komunikasi serial dengan kecepatan 9600 bps
  Serial.begin(9600);
  
  // Set pin trigPin sebagai output
  pinMode(trigPin, OUTPUT);
  // Set pin echoPin sebagai input
  pinMode(echoPin, INPUT);

  // Hubungkan ke Wi-Fi dan Blynk
  checkWiFi();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  checkWiFi();
  Blynk.run();

  // Kirimkan sinyal ultrasonik
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Baca waktu perjalanan gelombang
  duration = pulseIn(echoPin, HIGH);
  
  // Hitung jarak berdasarkan waktu perjalanan
  unsigned int distance = duration * 0.034 / 2;

  // Kirim data jarak ke Blynk
  Blynk.virtualWrite(V0, distance);  // Gunakan V0 sebagai widget display di aplikasi Blynk
  
  // Tampilkan jarak dalam centimeter
  Serial.print("Jarak: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  // Tunggu sejenak sebelum mengukur kembali
  delay(1000);
}
