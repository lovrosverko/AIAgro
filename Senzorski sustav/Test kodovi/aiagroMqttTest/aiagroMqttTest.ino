#include <WiFi.h>
#include <PubSubClient.h>

// WiFi
const char* ssid = "Sedmica";
const char* password = "!sedmica!";

// MQTT
const char* mqttServer = "7.7.7.119";
const int mqttPort = 1883;

// MQTT klijent
WiFiClient espClient;
PubSubClient mqttClient(espClient);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi spojen!");

  // MQTT
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(callback);
}

void loop() {
  // Čitaju se vrijednosti senzora
  String padaKisa = Serial.readStringUntil('\n');
  String soilMoisture = Serial.readStringUntil('\n');
  String humidity = Serial.readStringUntil('\n');
  String temperature = Serial.readStringUntil('\n');
  String uvValue = Serial.readStringUntil('\n');

  // Ako je MQTT klijent spojen, šalju se podaci
  if (mqttClient.connected()) {
    mqttClient.publish("aiagro/padaKisa", padaKisa.c_str());
    mqttClient.publish("aiagro/soilMoisture", soilMoisture.c_str());
    mqttClient.publish("aiagro/humidity", humidity.c_str());
    mqttClient.publish("aiagro/temperature", temperature.c_str());
    mqttClient.publish("aiagro/uvValue", uvValue.c_str());
  }

  // Povezivanje s MQTT brokerom ako nije već spojen
  if (!mqttClient.connected()) {
    while (!mqttClient.connect("ESP32", "", "")) {
      Serial.print("Povezivanje s MQTT brokerom nije uspjelo!");
      delay(5000);
    }
    Serial.println("Povezan s MQTT brokerom!");
  }
  mqttClient.loop();
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Obrada poruka koje dolaze na MQTT topic
  Serial.print("Primljena poruka na topicu: ");
  Serial.println(topic);
  Serial.print("Poruka: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
