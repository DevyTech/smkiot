#include "esp_camera.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#define CAMERA_MODEL_AI_THINKER // Has PSRAM
#include "camera_pins.h"

// Your Wi-Fi credentials
char ssid[] = "LABRPS";
char pass[] = "Komputer1111";

// Create a web server object that listens for HTTP request on port 80
WebServer server(81);
// Handle root URL
void handleRoot() {
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  String response = "hello";
  server.send(200, "text/html", response);
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
void startCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_SVGA; // Ubah ukuran sesuai kebutuhan (e.g., FRAMESIZE_QVGA, FRAMESIZE_SVGA)
  config.jpeg_quality = 4; // Kualitas JPEG (nilai lebih kecil = kualitas lebih baik)
  config.fb_count = 1;

  // Start camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  Serial.println("Camera initialized successfully");
}
// void handleStream() { // menangkap gambar terbaru
//   camera_fb_t * fb = NULL;
//   fb = esp_camera_fb_get();
//   if (!fb) {
//     Serial.println("Camera capture failed");
//     server.send(503, "text/plain", "Camera capture failed");
//     return;
//   }

//   // Kirim header HTTP
//   WiFiClient client = server.client();
//   client.println("HTTP/1.1 200 OK");
//   client.println("Content-Type: image/jpeg");
//   client.println("Content-Length: " + String(fb->len));
//   client.println();

//   // Kirim buffer gambar
//   client.write(fb->buf, fb->len);

//   // Kembalikan framebuffer ke kamera
//   esp_camera_fb_return(fb);
// }

unsigned long startTime;
const unsigned long timeoutPeriod = 60000; // 60 detik timeout

void handleStream() {
  camera_fb_t * fb = NULL;
  WiFiClient client = server.client();

  startTime = millis(); // Atur waktu mulai

  if (!client.connected()) {
    return;
  }

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: multipart/x-mixed-replace; boundary=frame");
  client.println();

  while (client.connected()) {
    if (millis() - startTime > timeoutPeriod) { // Cek timeout
      Serial.println("Timeout terjadi, menutup koneksi.");
      client.stop(); // Tutup koneksi jika timeout
      return;
    }

    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      continue;
    }

    // Kirim boundary
    client.println("--frame");
    client.println("Content-Type: image/jpeg");
    client.println("Content-Length: " + String(fb->len));
    client.println();
    client.write(fb->buf, fb->len);
    client.println();
    esp_camera_fb_return(fb);

    delay(30); // Sesuaikan untuk FPS
  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // Hubungkan ke Wi-Fi dan Webserver
  checkWiFi();
  // Print the IP address
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialize mDNS
  if(!MDNS.begin("camera")){ // Set the hostname to "camera.local"
    Serial.println("Error setting up MDNS responder!");
    while(1){
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  delay(2000);
  startCamera();
  // Initialize web server
  server.on("/", handleRoot);
  server.on("/stream", HTTP_GET, handleStream);
  server.begin();
  Serial.println("HTTP server started in Port 81");
}

void loop() {
  // put your main code here, to run repeatedly:
  checkWiFi();
  
  // Handle client requests
  server.handleClient();
}
