#include "DHT.h"

#define DHTPIN 14     // Pin DHT11 terhubung ke pin GPIO2 (atau D2)
#define DHTTYPE DHT11   // Tipe sensor DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  delay(2000); // Tunggu 2 detik antara pembacaan

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Gagal membaca sensor DHT11!");
    return;
  }

  Serial.print("Kelembaban: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Suhu: ");
  Serial.print(temperature);
  Serial.println(" °C");
}
