#include "DHT.h"
#define DHTPIN 14
#define DHTTYPE DHT11
#include <WiFi.h>
#include <WiFiClientSecure.h> // Tambahkan include ini
#include <ESP32Servo.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer-esphttps.h> // Tambahkan include ini
#include <ESPmDNS.h>
#include <ArduinoJson.h>

// Sertifikat dan kunci pribadi (ganti dengan data Anda)
#include "cert.h"
#include "private_key.h"

// Your Wi-Fi credentials
char ssid[] = "LABRPS";
char pass[] = "Komputer1111";

// Create a web server object that listens for HTTPS request on port 443
AsyncWebServerSecure server(443); // Port 443 untuk HTTPS

//Sensor Pin
// Ultrasoonic HC-SR04
const int trigPin = 13;
const int echoPin = 12;

// DH11
DHT dht(DHTPIN, DHTTYPE);

// Gas
const int MQ2_PIN_DO = 26; // Pin sensor gas MQ-2 terhubung ke pin 26

// Smoke

const int SMOKE_PIN = 32;

// Servo
static const int servoGarasi = 27;
static const int servoPintu = 34;
static const int servoJendela = 5;

Servo servo1,servo2,servo3;

bool servoPintuState = false;
bool servoJendelaState = false;

// Rain
const int RAIN_SENSOR_PIN = 25;

// Buzzer
int BUZZER_PIN = 33; // deklarasi pin buzzer

// LED
const int LED_TERAS = 15;
const int LED_TENGAH = 4;
const int LED_KAMAR1 = 18;
const int LED_KAMAR2 = 19;
const int LED_DAPUR = 21;
const int LED_GARASI = 22;
bool stateTeras = false;
bool stateTengah = false;
bool stateKamar1 = false;
bool stateKamar2 = false;
bool stateDapur = false;
bool stateGarasi = false;

// Handle root URL
void handleRoot(AsyncWebServerRequest *request) {
  request->send(200, "text/html", "online");
}

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

void ultrasonic(AsyncWebServerRequest *request){
  // Variabel untuk menyimpan waktu perjalanan gelombang ultrasonik
  long duration;
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
  String servoVal = distance >= 10 ? "Servo Tertutup" : "Servo Terbuka";
  servo1.write(distance >= 10 ? 90 : 0);
  // Kirim data jarak ke webserver
  String jsonResponse = "{\"distance\": " + String(distance) + ", \"servo\": \"" + servoVal + "\"}";
  request->send(200, "application/json", jsonResponse);
  // Tampilkan jarak dalam centimeter
  Serial.print("Jarak: ");
  Serial.print(distance);
  Serial.print(" cm | ");
  Serial.println(servoVal);
}
void kelembapan(AsyncWebServerRequest *request){

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Gagal membaca sensor DHT11!");
  }

  // Kirim data teperature dan kelembapan ke webserver
  String kelembapan = "{\"temperature\": " + String(temperature) + ", \"humidity\": " + String(humidity) + "}";
  request->send(200, "application/json", kelembapan);

  Serial.print("Kelembaban: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Suhu: ");
  Serial.print(temperature);
  Serial.println(" °C");
}
// ... (Kode fungsi lainnya sama)

void setup() {
  // ... (kode setup lainnya)

  // Muat sertifikat dan kunci pribadi
  server.useRSACert(example_crt_DER, example_crt_DER_len, example_key_DER, example_key_DER_len);

  // Hubungkan ke Wi-Fi dan Webserver
  checkWiFi();
  // Print the IP address
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialize mDNS
  if(!MDNS.begin("stmiot")){ // Set the hostname to "stmiot.local"
    Serial.println("Error setting up MDNS responder!");
    while(1){
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");

  // Initialize web server
  server.on("/",HTTP_GET, handleRoot);
  server.on("/jarak", ultrasonic);
  server.on("/kelembapan", kelembapan);
  // ... (kode endpoint lainnya)
  server.begin();
  Serial.println("HTTPS server started on port 443");

  Serial.println("Warming up the MQ2 sensor");
  delay(10000); // wait for the MQ2 to warm up
  Serial.println("Device Ready");
}

void loop() {
  // put your main code here, to run repeatedly:
  checkWiFi();
}