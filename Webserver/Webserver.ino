#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h> // Include mDNS library

// Replace with your network credentials
const char* ssid = "LABRPS";
const char* password = "Komputer0000";

// Create a web server object that listens for HTTP request on port 80
WebServer server(80);

bool ledState = false;

void handleRoot() {
  server.send(200, "text/html", "Hello from ESP32");
}

void handleLEDToggle() {
  ledState = !ledState;
  digitalWrite(18, ledState ? HIGH : LOW);
  server.send(200, "text/plain", ledState ? "ON" : "OFF");
}

void handleGetLEDStatus() {
  String status = ledState ? "ON" : "OFF";
  server.send(200, "application/json", "{\"ledState\": \"" + status + "\"}");
}

void setup() {
  Serial.begin(9600);
  pinMode(18, OUTPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Start mDNS service
  if (!MDNS.begin("esp32")) { // Replace "esp32" with the desired hostname
    Serial.println("Error starting mDNS");
    return;
  }
  Serial.println("mDNS responder started");

  // Print the hostname for access
  Serial.print("Access ESP32 at http://");
  Serial.print("esp32"); // Replace "esp32" with the desired hostname
  Serial.println(".local");

  // Initialize web server
  server.on("/", handleRoot);
  server.on("/toggle-led", handleLEDToggle);
  server.on("/get-led-status", handleGetLEDStatus);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
