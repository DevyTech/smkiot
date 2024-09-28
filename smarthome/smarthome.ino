#include "DHT.h"
#define DHTPIN 14
#define DHTTYPE DHT11
#include <WiFi.h>
#include <WiFiClient.h>
#include <ESP32Servo.h>
#include <WebServer.h>
#include <ESPmDNS.h>

// Your Wi-Fi credentials
char ssid[] = "LABRPS";
char pass[] = "Komputer1111";

// Create a web server object that listens for HTTP request on port 80
WebServer server(80);

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
static const int servoPintu = 4;

Servo servo1,servo2;

bool servoPintuState = false;

// Rain
const int RAIN_SENSOR_PIN = 25;

// Buzzer
int BUZZER_PIN = 33; // deklarasi pin buzzer

// LED
const int LED_PIN = 18;
bool ledState = false;

// Handle root URL
void handleRoot() {
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  String sensorState = "online";
  server.send(200, "text/html", sensorState);
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

void ultrasonic(){
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
  String servoVal;
  if(distance>=10){
    servo1.write(90);
    servoVal = "Servo Tertutup";
  }else{
    servoVal = "Servo Terbuka";
    servo1.write(0);
  }
  // Kirim data jarak ke webserver
  String jsonResponse = "{\"distance\": " + String(distance) + ", \"servo\": \"" + servoVal + "\"}";
  server.send(200,"application/json",jsonResponse);
  // Tampilkan jarak dalam centimeter
  Serial.print("Jarak: ");
  Serial.print(distance);
  Serial.print(" cm | ");
  Serial.println(servoVal);
}
void kelembapan(){

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Gagal membaca sensor DHT11!");
  }

  // Kirim data teperature dan kelembapan ke webserver
  String kelembapan = "{\"temperature\": " + String(temperature) + ", \"humidity\": " + String(humidity) + "}";
  server.send(200, "application/json", kelembapan);

  Serial.print("Kelembaban: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Suhu: ");
  Serial.print(temperature);
  Serial.println(" °C");
}
void gas(){
  int gasState = digitalRead(MQ2_PIN_DO);
  String gasVal;

  if (gasState != HIGH){
    Serial.println("Gas Detected");
    gasVal = "Gas Terdeteksi";
    digitalWrite(BUZZER_PIN, LOW); // bunyikan buzzer
  }else{
    gasVal = "Gas Tidak Terdeteksi";
    digitalWrite(BUZZER_PIN, HIGH); // matikan buzzer
  }
  server.send(200,"text/plain",gasVal);
}

void smoke(){
  int smokeState = digitalRead(SMOKE_PIN);
  String smokeVal;

  if(smokeState != HIGH){
    Serial.println("Smoke Detected");
    smokeVal = "Asap Terdeteksi";
    digitalWrite(BUZZER_PIN, LOW); // bunyikan buzzer
  }else{
    smokeVal = "Asap Tidak Terdeteksi";
    digitalWrite(BUZZER_PIN, HIGH); // matikan buzzer
  }
  server.send(200,"text/plain",smokeVal);
}


void hujan(){
  delay(10);
  int rain_state = digitalRead(RAIN_SENSOR_PIN);  // Membaca nilai dari sensor hujan
  String rainVal;
  if (rain_state != HIGH) {  // Jika tidak ada hujan
    Serial.println("Rain Detected.");
    rainVal = "Air Terdeteksi";
    // Buzzer ON (LOW)
    digitalWrite(BUZZER_PIN, LOW); // bunyikan buzzer
    delay(250); // delay 250 milidetik

    // Buzzer OFF (HIGH)
    digitalWrite(BUZZER_PIN, HIGH); // matikan buzzer
    delay(250); // delay 250 milidetik
  }else{
    rainVal = "Air Tidak Terdeteksi";
    digitalWrite(BUZZER_PIN, HIGH); // matikan buzzer
  }
  server.send(200,"text/plain",rainVal);
}
// Handle Servo Pintu toggle URL
void handleServoPintu(){
  servoPintuState = !servoPintuState;
  servo2.write(servoPintuState ? 90 : 0);
  server.send(200, "text/plain", servoPintuState ? "Pintu Terbuka" : "Pintu Tertutup");
  Serial.print("Servo :");
  Serial.println(servoPintuState);
}
void handleGetPintuStatus(){
  String status = servoPintuState ? "Pintu Terbuka" : "Pintu Tertutup";
  server.send(200, "application/json", "{\"servoPintuState\": \"" + status + "\"}");
  Serial.print("Status :");
  Serial.println(status);
}
// Handle LED toggle URL
void handleLEDToggle() {
  ledState = !ledState;
  digitalWrite(18, ledState ? HIGH : LOW);
  server.send(200, "text/plain", ledState ? "ON" : "OFF");
}
// Handle current LED status URL
void handleGetLEDStatus() {
  String status = ledState ? "ON" : "OFF";
  server.send(200, "application/json", "{\"ledState\": \"" + status + "\"}");
}
void setup() {
  // put your setup code here, to run once:
  // Mulai komunikasi serial dengan kecepatan 9600 bps
  Serial.begin(9600);

  //LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // turn the LED off

  // Ultrasonic
  // Set pin trigPin sebagai output
  pinMode(trigPin, OUTPUT);
  // Set pin echoPin sebagai input
  pinMode(echoPin, INPUT);

  //DHT11
  dht.begin();

  //MQ2 Gas
  pinMode(MQ2_PIN_DO, INPUT);

  //MQ2 Smoke
  pinMode(SMOKE_PIN, INPUT);

  //Servo
  servo1.attach(servoGarasi);
  servo2.attach(servoPintu);

  // Rain
  pinMode(RAIN_SENSOR_PIN, INPUT);

  // Buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, HIGH); //Matikan Buzzer

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
  server.on("/", handleRoot);
  // server.on("/jarak", ultrasonic);
  // server.on("/kelembapan", kelembapan);
  // server.on("/gas", gas);
  // server.on("/hujan", hujan);
  // server.on("/asap",smoke);
  // server.on("/toggle-led", handleLEDToggle);
  // server.on("/get-led-status", handleGetLEDStatus);
  server.on("/servoPintu",handleServoPintu);
  server.on("/servoPintuStatus",handleGetPintuStatus);
  server.begin();
  Serial.println("HTTP server started");
  
  Serial.println("Warming up the MQ2 sensor");
  delay(10000);  // wait for the MQ2 to warm up
  Serial.println("Device Ready");
  //digitalWrite(BUZZER_PIN, LOW); //Bunyikan Buzzer
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  checkWiFi();
  
  // Handle client requests
  server.handleClient();
  //delay(1000); // Tunggu 1 detik antara pembacaan
}
