#include "DHT.h"
#define DHTPIN 14
#define DHTTYPE DHT11
#include <WiFi.h>
#include <WiFiClient.h>
#include <ESP32Servo.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>

// Your Wi-Fi credentials
char ssid[] = "LABRPS";
char pass[] = "Komputer1111";

// Create a web server object that listens for HTTP request on port 80
AsyncWebServer server(8080);

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
static const int servoPintu = 34; //can use 34 or 35 pin

Servo servo1,servo2;

bool servoPintuState = false;

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
void gas(AsyncWebServerRequest *request){
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
  request->send(200, "text/plain", gasVal);
}

void smoke(AsyncWebServerRequest *request){
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
  request->send(200,"text/plain",smokeVal);
}


void hujan(AsyncWebServerRequest *request){
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
  request->send(200,"text/plain",rainVal);
}
// Handle Servo Pintu toggle URL
void handleServoPintu(AsyncWebServerRequest *request){
  servoPintuState = !servoPintuState;
  servo2.write(servoPintuState ? 90 : 0);
  request->send(200, "text/plain", servoPintuState ? "Pintu Terbuka" : "Pintu Tertutup");
  Serial.print("Servo :");
  Serial.println(servoPintuState);
}
void handleGetPintuStatus(AsyncWebServerRequest *request){
  String status = servoPintuState ? "Pintu Terbuka" : "Pintu Tertutup";
  request->send(200, "application/json", "{\"servoPintuState\": \"" + status + "\"}");
  Serial.print("Status :");
  Serial.println(status);
}
// Handle LED toggle URL
void toggleLED(int pin, bool &state, AsyncWebServerRequest *request) {
  state = !state;
  digitalWrite(pin, state ? HIGH : LOW);
  String ledStatus = state ? "ON" : "OFF";
  request->send(200, "text/plain", ledStatus);
  Serial.print("LED on pin ");
  Serial.print(pin);
  Serial.print(" is now ");
  Serial.println(ledStatus);
}
// Handle current LED status URL
void handleLEDToggle(AsyncWebServerRequest *request) {
  if (request->hasParam("led")){
    String led = request->getParam("led")->value();
    if(led=="teras"){
      toggleLED(LED_TERAS, stateTeras, request);
    }else if(led=="tengah"){
      toggleLED(LED_TENGAH, stateTengah, request);
    }else if(led=="kamar1"){
      toggleLED(LED_KAMAR1, stateKamar1, request);
    }else if(led=="kamar2"){
      toggleLED(LED_KAMAR2, stateKamar2, request);
    }else if(led=="dapur"){
      toggleLED(LED_DAPUR, stateDapur, request);
    }else if(led=="garasi"){
      toggleLED(LED_GARASI, stateGarasi, request);
    }else{
      request->send(400,"text/plain","Invalid LED");
    }
  }else{
    request->send(400,"text/plain","No LED specified");
  }
}
void handleGetLEDStatus(AsyncWebServerRequest *request) {
  String statusTeras = stateTeras ? "ON" : "OFF";
  String statusTengah = stateTengah ? "ON" : "OFF";
  String statusKamar1 = stateKamar1 ? "ON" : "OFF";
  String statusKamar2 = stateKamar2 ? "ON" : "OFF";
  String statusDapur = stateDapur ? "ON" : "OFF";
  String statusGarasi = stateGarasi ? "ON" : "OFF";

  request->send(200, "application/json", "{\"stateTeras\": \"" + statusTeras + "\", \"stateTengah\": \"" + statusTengah + "\", \"stateKamar1\": \"" + statusKamar1 + "\", \"stateKamar2\": \"" + statusKamar2 + "\", \"stateDapur\": \"" + statusDapur + "\", \"stateGarasi\": \"" + statusGarasi + "\"}");
}
void setup() {
  // put your setup code here, to run once:
  // Mulai komunikasi serial dengan kecepatan 9600 bps
  Serial.begin(9600);

  //LED
  pinMode(LED_TERAS, OUTPUT);
  pinMode(LED_TENGAH, OUTPUT);
  pinMode(LED_KAMAR1, OUTPUT);
  pinMode(LED_KAMAR2, OUTPUT);
  pinMode(LED_DAPUR, OUTPUT);
  pinMode(LED_GARASI, OUTPUT);

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
  server.on("/",HTTP_GET, handleRoot);
  server.on("/jarak", ultrasonic);
  server.on("/kelembapan", kelembapan);
  server.on("/gas", gas);
  server.on("/hujan", hujan);
  server.on("/asap",smoke);
  server.on("/toggle-led",HTTP_GET, handleLEDToggle);
  server.on("/get-led-status",HTTP_GET, handleGetLEDStatus);
  server.on("/servoPintu", HTTP_GET, handleServoPintu);
  server.on("/servoPintuStatus",HTTP_GET, handleGetPintuStatus);
  server.begin();
  Serial.println("HTTP server started in Port 8080");
  
  Serial.println("Warming up the MQ2 sensor");
  delay(10000);  // wait for the MQ2 to warm up
  Serial.println("Device Ready");
  //digitalWrite(BUZZER_PIN, LOW); //Bunyikan Buzzer
}

void loop() {
  // put your main code here, to run repeatedly:
  checkWiFi();   
  //delay(1000); // Tunggu 1 detik antara pembacaan
}
