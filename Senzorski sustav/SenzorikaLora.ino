/*
** Autor: Centar Pozitron**
** Naziv projekta: AIAgro**
**
** Ovaj kod implementira sustav za praćenje okoline s nizom senzora i 
** slanje podataka putem LoRa modula. 
**
** Senzori koji se koriste su:**
** - Senzor kiše (analogni)
** - Senzor vlažnosti zemlje (analogni)
** - DHT senzor (temperatura i vlažnost zraka)
** - UV senzor (analogni)
**
** Podaci se periodično čitaju iz senzora i šalju LoRa modulom na 
** udaljeni prijemnik. 
**
** Verzija: 3.0**
** Datum: 26.04.2024.**
*/

#include <AnalogUVSensor.h>
#include <SPI.h>
#include "LoRa.h"
#include "DHT.h"  // Uključuje DHT knjižnicu

#define LORA
#define DHTTYPE DHT11  // Definira tip DHT senzora

#define RAINPIN A0  // Analogni pin za detekciju kiše
#define SOILPIN A1  // Analogni pin za mjerenje vlažnosti zemlje
#define DHTPIN A2   // Digitalni pin za DHT senzor
#define UVPIN A3    // Analogni pin za UV senzor

// Lora pin definicije
const int csPin = 10;
const int resetPin = 4;
const int irqPin = 2;

int counter = 0;      // Brojač poslanih paketa informacija
int padaKisa = 0;     // Varijabla za detekciju kiše

AnalogUVSensor AUV;   // Stvara objekt klase za rad s UV senzorom
DHT dht(DHTPIN, DHTTYPE); // Stvara objekt klase za rad s DHT senzorom

void setup() {
  Serial.begin(9600);   // Inicijalizacija serijske komunikacije
  pinMode(RAINPIN, INPUT);  // Postavlja pin za senzor kiše kao ulazni
  pinMode(SOILPIN, INPUT);  // Postavlja pin za senzor vlažnosti kao ulazni
  pinMode(DHTPIN, INPUT);   // Postavlja pin za DHT kao ulazni
  pinMode(UVPIN, INPUT);    // Postavlja pin za UV senzor kao ulazni

  while (!Serial);          // Čekaj dok serijska veza nije uspostavljena

  Serial.println("LoRa Sender"); // Ispis na serijski port

  LoRa.setPins(csPin, resetPin, irqPin); // Postavljanje LoRa pinova

  if (!LoRa.begin(868E6)) {   // Pokušaj inicijalizacije LoRa modula
    Serial.println("Starting LoRa failed!");  // Upozorenje u slučaju greške
    while (1); // Beskonačna petlja jer nema smisla nastavljati bez LoRa modula
  }

  AUV.begin(UVPIN, 3.3, 1023); // Inicijalizacija UV senzora 
  dht.begin();                 // Inicijalizacija DHT senzora 
}

void loop() {
  // DHT senzor (vlaga i temperatura zraka)
  float humidity = dht.readHumidity();  
  float temperature = dht.readTemperature(); 

  Serial.print("H: ");  // Ispis oznake za vlagu
  Serial.println(humidity);
  Serial.print("T: ");  // Ispis oznke za temperaturu
  Serial.println(temperature);

  // Senzor kiše
  int rainValue = analogRead(RAINPIN);  
  Serial.print("Rain: "); 
  Serial.println(rainValue);

  // Provjera pada li kiša - ako je analogna vrijednost ispod 350 smatramo da pada 
  if (rainValue < 350) {
    padaKisa = 1; 
  } else {
    padaKisa = 0; 
  }
  Serial.print("Pada kiša: ");
  Serial.println(padaKisa);

  // Senzor vlažnosti zemlje
  int soilMoisture = analogRead(SOILPIN); 
  Serial.print("Soil moisture: ");

  // Pretvorba analognog signala u postotak (0-100%) 
  soilMoisture = map (soilMoisture, 1032, 0, 0, 100); 
  Serial.println(soilMoisture);

  // UV senzor
  float uvValue = AUV.read(); 
  Serial.print("UV razina ");
  Serial.println(uvValue);

  // Slanje paketa s informacijama senzora putem LoRa protokola
  LoRa.beginPacket(); 
  LoRa.println(padaKisa);
  LoRa.println(soilMoisture);
  LoRa.println(humidity);
  LoRa.println(temperature); 
  LoRa.println(uvValue);
  LoRa.endPacket(); 

  delay(2000); // Pauza od 2 sekunde između slanja poruka 
}

// Funkcija za izračun UV indeksa na temelju izmjerene analogne vrijednosti
float indexUV(uint16_t pin, int16_t analog_max, float voltage_max) {
  // Čitanje analogne vrijednosti sa senzora 
  uint16_t raw = analogRead(pin); 

  // Konverzija u milivolte
  float millivolt = (voltage_max * raw * 1000) / analog_max; 

  /* Ovisno o razini milivolta računa se UV index (linearnim formulama 
     koje aproksimiraju stvarne vrijednosti senzora ) */

  if (millivolt < 50) { 
    return 0.0; // Zanemarujemo vrijednosti ispod 50mV kao šum
  }
  if (millivolt < 227) {
    float uvi = 0.0 + (millivolt - 50.0) / (227.0 - 50.0); 
    return uvi; 
  }
  float uvi = 0.0104865310 * millivolt - 1.289154988; 
  return uvi;
}
