#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

// === Konfigurasi WiFi ===
const char* ssid = "Wokwi-GUEST";  
const char* password = "";

// === Konfigurasi MQTT ===
const char* mqtt_server = "test.mosquitto.org"; 
const char* mqtt_topic = "gas/rumah";           

// === Pin dan Tipe Sensor ===
#define DHTPIN 21
#define DHTTYPE DHT11
#define GAS_SENSOR_PIN 34
#define LED_PIN 2
#define BUZZER_PIN 15

// Ambang batas
int gasThreshold = 2000;
float tempThreshold = 40.0; 

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

void setup_wifi() {
  Serial.print("Menghubungkan ke WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi terhubung");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke MQTT...");
    if (client.connect("ESP32ClientGas")) {
      Serial.println("Terhubung");
    } else {
      Serial.print("Gagal, rc=");
      Serial.print(client.state());
      Serial.println(" mencoba lagi dalam 5 detik");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Monitoring DHT11 + MQ-2 + LED + Buzzer + MQTT");

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // ==== Baca DHT11 ====
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Gagal membaca data dari DHT11!");
    return;
  }

  // ==== Baca MQ-2 ====
  int gasValue = analogRead(GAS_SENSOR_PIN);

  // ==== Alarm ====
  if (gasValue > gasThreshold || temperature > tempThreshold) {
    digitalWrite(LED_PIN, HIGH);
    for (int i = 0; i < 3; i++) {
      digitalWrite(BUZZER_PIN, HIGH);
      delay(200);
      digitalWrite(BUZZER_PIN, LOW);
      delay(200);
    }
  } else {
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }

  // ==== JSON sederhana ====
  char payload[100];
  sprintf(payload, "{\"temperature\":%.2f,\"humidity\":%.2f,\"gas\":%d}",
          temperature, humidity, gasValue);

  client.publish(mqtt_topic, payload);

  Serial.print("Data terkirim: ");
  Serial.println(payload);

  delay(2000);
}
