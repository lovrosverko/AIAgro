/*
** Autor: Centar Pozitron** 
** Datum: 26.4.2024. ** 
** Projekt:** AIAgro

** Opis:**

Ovaj kod implementira komunikaciju ESP32 mikrokontrolera s MQTT brokerom koristeći WiFi i MQTT biblioteke. 
Mikrokontroler se spaja na WiFi mrežu s imenom "Sedmica" i lozinkom "********". 
Nakon spajanja na WiFi, mikrokontroler se povezuje na MQTT broker s adresom "7.7.7.119" i standardnim MQTT portom 1883.

** Funkcionalnost:**

* **Čitanje podataka:** 
    * Kod trenutno ne čita podatke s senzora, već simulira čitanje tako što čita podatke s serijske veze.
    * Čitaju se sljedeći podaci:
        * Padalina (padaKisa)
        * Vlažnost tla (soilMoisture)
        * Vlažnost zraka (humidity)
        * Temperatura (temperature)
        * Vrijednost UV zračenja (uvValue)
* **Slanje podataka:** 
    * Pročitani podatci se šalju na MQTT broker na sljedeće topicse:
        * aiagro/padaKisa
        * aiagro/soilMoisture
        * aiagro/humidity
        * aiagro/temperature
        * aiagro/uvValue
        * aiagro/test (Dodana testna poruka)
* **Primanje poruka:** 
    * Kod prati poruke na svim topicsima i ispisuje ih na serijski monitor.

**Dodatne napomene:**

* Kod koristi `callback` funkciju za obradu MQTT poruka.
* U `loop` funkciji se provjerava je li mikrokontroler spojen na MQTT broker. Ako nije, pokušava se spojiti.
* Kod sadrži i komentar koji ukazuje da se za čitanje podataka s senzora trebaju implementirati funkcionalnosti.

**Primjer upotrebe:**

Ovaj kod se koristi za prikupljanje podataka s različitih senzora i 
slanje tih podataka na MQTT broker za daljnju obradu ili vizualizaciju. 
*/

#include <WiFi.h>          // Uključuje WiFi biblioteku za komunikaciju preko WiFi mreže
#include <PubSubClient.h>  // Uključuje MQTT biblioteku za komunikaciju s MQTT brokerom

// WiFi podaci
const char* ssid = "Sedmica";      // Ime WiFi mreže na koju se spajamo
const char* password = "*******";  // Lozinka WiFi mreže

// MQTT podaci
const char* mqttServer = "7.7.7.119";  // Adresa MQTT brokera
const int mqttPort = 1883;             // Standardni port koji koristi MQTT protokol

// Stvaranje MQTT klijenta
WiFiClient espClient;                // WiFi klijent potreban za komunikaciju
PubSubClient mqttClient(espClient);  // MQTT klijent koji će komunicirati s brokerom

// Varijable za pohranu podataka senzora
String padaKisa;
String soilMoisture;
String humidity;
String temperature;
String uvValue;

void setup() {
  Serial.begin(9600);  // Inicijalizacija serijske veze za komunikaciju s računalom
  while (!Serial)
    ;           // Čeka dok serijska veza nije spremna
  delay(2000);  // Sanity check

  // WiFi povezivanje
  WiFi.begin(ssid, password);              // Započinje spajanje na WiFi mrežu
  while (WiFi.status() != WL_CONNECTED) {  // Provjera je li spajanje uspješno
    delay(500);                            // Čeka 0.5 sekundi
    Serial.print(".");                     // Ispisuje točku na serijski monitor
  }
  Serial.println("");              // Prazan redak
  Serial.println("WiFi spojen!");  // Ispisuje potvrdu uspješnog spajanja

  // MQTT postavljanje
  mqttClient.setServer(mqttServer, mqttPort);  // Postavljanje adrese i porta MQTT brokera
  mqttClient.setCallback(callback);            // Postavljanje funkcije za obradu poruka
}

void loop() {
  // čitanje podataka sa serijske veze, logičko razdvajanje i spremanje u posebne varijable
  if (Serial.available()) {
    padaKisa = Serial.readStringUntil('\n');
    soilMoisture = Serial.readStringUntil('\n');
    humidity = Serial.readStringUntil('\n');
    temperature = Serial.readStringUntil('\n');
    uvValue = Serial.readStringUntil('\n');
    String dummy = Serial.readStringUntil('\n');  // Ova linija je dodana za ignoriranje dodatnog znaka za novi red

    if (mqttClient.connected()) {                                       // Provjerava je li klijent spojen na MQTT broker
      mqttClient.publish("aiagro/padaKisa", padaKisa.c_str());          // Šalje podatke o padalinama na topic "aiagro/padaKisa"
      mqttClient.publish("aiagro/soilMoisture", soilMoisture.c_str());  // Šalje podatke o vlažnosti tla na topic "aiagro/soilMoisture"
      mqttClient.publish("aiagro/humidity", humidity.c_str());          // Šalje podatke o vlažnosti zraka na topic "aiagro/humidity"
      mqttClient.publish("aiagro/temperature", temperature.c_str());    // Šalje podatke o temperaturi na topic "aiagro/temperature"
      mqttClient.publish("aiagro/uvValue", uvValue.c_str());            // Šalje podatke o UV zračenju na topic "aiagro/uvValue"
      mqttClient.publish("aiagro/test", "Radi");                        // Šalje testnu poruku "Radi" na topic "aiagro/test"
    }
  }

  // Pokušaj (re)spajanja na MQTT broker ako nismo spojeni
  if (!mqttClient.connected()) {
    while (!mqttClient.connect("ESP32", "", "")) {  // Pokušaj spajanja dok ne uspije
      Serial.print("Povezivanje s MQTT brokerom nije uspjelo!");
      delay(5000);  // Čekaj 5 sekundi između pokušaja
    }
    Serial.println("Povezan s MQTT brokerom!");
  }
  mqttClient.loop();  // Obrada MQTT komunikacije (primanje poruka...)
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
