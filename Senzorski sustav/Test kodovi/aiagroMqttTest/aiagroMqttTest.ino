#include <WiFi.h> // Uključuje WiFi biblioteku za komunikaciju preko WiFi mreže
#include <PubSubClient.h> // Uključuje MQTT biblioteku za komunikaciju s MQTT brokerom

// WiFi podaci
const char* ssid = "Sedmica";         // Ime WiFi mreže na koju se spajamo
const char* password = "!sedmica!";   // Lozinka WiFi mreže

// MQTT podaci 
const char* mqttServer = "7.7.7.119"; // Adresa MQTT brokera
const int mqttPort = 1883;            // Standardni port koji koristi MQTT protokol

// Stvaranje MQTT klijenta
WiFiClient espClient;                 // WiFi klijent potreban za komunikaciju
PubSubClient mqttClient(espClient);   // MQTT klijent koji će komunicirati s brokerom  

void setup() {
  Serial.begin(9600);  // Inicijalizacija serijske veze za komunikaciju s računalom
  while (!Serial);     // Čeka dok serijska veza nije spremna 

  // WiFi povezivanje
  WiFi.begin(ssid, password);                // Započinje spajanje na WiFi mrežu
  while (WiFi.status() != WL_CONNECTED) {    // Provjera je li spajanje uspješno
    delay(500);                              // Čeka 0.5 sekundi
    Serial.print(".");                       // Ispisuje točku na serijski monitor
  }
  Serial.println("");                        // Prazan redak
  Serial.println("WiFi spojen!");            // Ispisuje potvrdu uspješnog spajanja

  // MQTT postavljanje
  mqttClient.setServer(mqttServer, mqttPort);  // Postavljanje adrese i porta MQTT brokera
  mqttClient.setCallback(callback);            // Postavljanje funkcije za obradu poruka
}

void loop() {
  // Čitanje senzora (OVDJE UMJESTO SENZORA ČITAMO PODATKE KOJI DOLAZE SERIJSKOM VEZOM)
  String padaKisa = Serial.readStringUntil('\n');   
  String soilMoisture = Serial.readStringUntil('\n'); 
  String humidity = Serial.readStringUntil('\n');    
  String temperature = Serial.readStringUntil('\n'); 
  String uvValue = Serial.readStringUntil('\n');     

  // Slanje podataka na MQTT broker samo ako smo spojeni
  if (mqttClient.connected()) {
    mqttClient.publish("aiagro/padaKisa", padaKisa.c_str());  // Objavljivanje na topic "padaKisa"
    mqttClient.publish("aiagro/soilMoisture", soilMoisture.c_str());
    mqttClient.publish("aiagro/humidity", humidity.c_str());
    mqttClient.publish("aiagro/temperature", temperature.c_str());
    mqttClient.publish("aiagro/uvValue", uvValue.c_str());
  }

  // Pokušaj (re)spajanja na MQTT broker ako nismo spojeni
  if (!mqttClient.connected()) { 
    while (!mqttClient.connect("ESP32", "", "")) { // Pokušaj spajanja dok ne uspije
      Serial.print("Povezivanje s MQTT brokerom nije uspjelo!"); 
      delay(5000); // Čekaj 5 sekundi između pokušaja
    }
    Serial.println("Povezan s MQTT brokerom!"); 
  }
  mqttClient.loop(); // Obrada MQTT komunikacije (primanje poruka...)
}

// Funkcija koja se poziva kada stigne poruka na neki MQTT topic 
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Primljena poruka na topicu: ");
  Serial.println(topic); 
  Serial.print("Poruka: ");
  for (int i = 0; i < length; i++) { 
    Serial.print((char)payload[i]);  // Ispis znaka poruke
  }
  Serial.println();
}
