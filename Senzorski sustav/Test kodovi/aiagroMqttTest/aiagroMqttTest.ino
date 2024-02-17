#include <WiFi.h>          // Biblioteka za WiFi funkcionalnosti
#include <PubSubClient.h>  // Biblioteka za rad s MQTT protokolom

// Zamijeni sljedeće vrijednosti podacima vlastite WiFi mreže
const char* ssid = "Sedmica";
const char* password = "!sedmica!";

// Zamijeni sljedeće s IP adresom MQTT brokera kojeg koristiš
const char* mqttServer = "7.7.7.119";
const int mqttPort = 1883;  // Standardni MQTT port

WiFiClient espClient;       // Stvaranje WiFi klijenta
PubSubClient client(espClient); // Stvaranje MQTT klijenta 

long lastPublishTime = 0;       // Varijabla za praćenje vremena zadnjeg slanja poruke
const long publishInterval = 2000;  // Interval objavljivanja poruka (2 sekunde)

void setup() {
  Serial.begin(115200);      // Inicijalizacija serijske komunikacije (za debugiranje)
  delay(2000);   // Pauza od 2 sekunde prije nastavka
  Serial.println("----------------START-----------------");

  WiFi.begin(ssid, password);   // Povezivanje na WiFi mrežu

  while (WiFi.status() != WL_CONNECTED) {  // Petlja čekanja dok se ne uspostavi WiFi veza
    delay(500);
    Serial.print(".");    // Vizualni pokazatelj čekanja
  }

  Serial.println("WiFi connected");  // Ispis potvrde kada je WiFi uspješno spojen
  delay(2000);           // Kratka pauza
  client.setServer(mqttServer, mqttPort); // Postavljanje adrese i porta MQTT brokera
}

void loop() {
  if (!client.connected()) {      // Ako nismo spojeni na MQTT broker...
    reconnect();                  // ...pokušaj ponovo uspostaviti vezu
  }
  client.loop();               // Osvježavanje stanja MQTT komunikacije

  long now = millis();            // Dohvaćanje trenutnog vremenskog trenutka

  if (now - lastPublishTime > publishInterval) { // Ako je prošlo dovoljno vremena...
    lastPublishTime = now;      // ...ažuriraj vrijeme zadnjeg objavljivanja

    // Objavljivanje tekstualne poruke:
    String stringMessage = "Test String Message";  
    client.publish("aiagro/testString", stringMessage.c_str()); 
    Serial.println("String message published");

    // Objavljivanje numeričke poruke (primjer):
    int numberMessage = 42;
    String numberMessageString = String(numberMessage);   
    client.publish("aiagro/testNumbers", numberMessageString.c_str());
    Serial.println("Number message published");
  }
}

void reconnect() { // Funkcija za ponovno spajanje na MQTT 
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect("AIAgro_bot")) {             // Pokušaj spajanja
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());               // Ako spajanje nije uspjelo, ispiši kod greške
      Serial.println(" Retrying in 5 seconds");
      delay(5000);                                // Pauza od 5 sekundi prije ponovnog pokušaja
    }
  }
}
