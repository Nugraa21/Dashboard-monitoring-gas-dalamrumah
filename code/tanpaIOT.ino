#include "DHT.h"

// === Pin dan Tipe Sensor ===
#define DHTPIN 4
#define DHTTYPE DHT11
#define GAS_SENSOR_PIN 34  // Pin analog MQ-2
#define LED_PIN 2
#define BUZZER_PIN 15

// Ambang batas kadar gas (0-4095 untuk ESP32 ADC)
int gasThreshold = 2000; // bisa diubah sesuai kebutuhan

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Monitoring DHT11 + MQ-2 + LED + Buzzer");

  dht.begin();
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  // ==== Baca DHT11 ====
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Gagal membaca data dari DHT11!");
  } else {
    Serial.print("Kelembapan: ");
    Serial.print(humidity);
    Serial.print(" %\tSuhu: ");
    Serial.print(temperature);
    Serial.println(" °C");
  }

  // ==== Baca MQ-2 ====
  int gasValue = analogRead(GAS_SENSOR_PIN);
  Serial.print("Kadar Gas: ");
  Serial.println(gasValue);

  // ==== Logika deteksi gas ====
  if (gasValue > gasThreshold) {
    // LED ON
    digitalWrite(LED_PIN, HIGH);

    // Buzzer bunyi bib-bib-bib
    for (int i = 0; i < 3; i++) {
      digitalWrite(BUZZER_PIN, HIGH);
      delay(200);
      digitalWrite(BUZZER_PIN, LOW);
      delay(200);
    }

  } else {
    // LED OFF dan buzzer mati
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }

  delay(1000);
}
