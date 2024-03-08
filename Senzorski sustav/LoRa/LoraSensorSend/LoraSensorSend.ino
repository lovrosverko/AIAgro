#include <AnalogUVSensor.h> // Biblioteka za UV senzor
#include <SPI.h> // Biblioteka za SPI komunikaciju
#include "LoRa.h" // Biblioteka za LoRa modul
#include "DHT.h" // Biblioteka za DHT senzor

#define LORA // Makro za korištenje LoRa modula
#define DHTTYPE DHT11 // Tip DHT senzora

#define RAINPIN A0 // Pin za senzor kiše
#define SOILPIN A1 // Pin za senzor vlage tla
#define DHTPIN A2 // Pin za DHT senzor
#define UVPIN A3 // Pin za UV senzor

const int csPin = 10; // Pin CS LoRa modula
const int resetPin = 4; // Pin reset LoRa modula
const int irqPin = 2; // Pin IRQ LoRa modula

int counter = 0; // Brojač impulsa kiše
int padaKisa = 0; // Indikator pada kiše (0/1)

AnalogUVSensor AUV; // Objekt UV senzora
DHT dht(DHTPIN, DHTTYPE); // Objekt DHT senzora

void setup() {
  // Inicijalizacija serijske komunikacije
  Serial.begin(9600);

  // Postavljanje pinova senzora kao ulaz
  pinMode(RAINPIN, INPUT);
  pinMode(SOILPIN, INPUT);
  pinMode(DHTPIN, INPUT);
  pinMode(UVPIN, INPUT);

  // Čekanje da se serijska komunikacija uspostavi
  while (!Serial);

  // Ispis poruke na serijski monitor
  Serial.println("LoRa Sender");

  // Postavljanje pinova LoRa modula
  LoRa.setPins(csPin, resetPin, irqPin);

  // Pokušaj inicijalizacije LoRa modula
  if (!LoRa.begin(868E6)) {
    // Ispis poruke o grešci
    Serial.println("Starting LoRa failed!");
    // Beskonačna petlja u slučaju greške
    while (1);
  }

  // Inicijalizacija UV senzora
  AUV.begin(UVPIN, 3.3, 1023);

  // Inicijalizacija DHT senzora
  dht.begin();
}

void loop() {
  // Čitanje vlage i temperature iz DHT senzora
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Ispis vlage i temperature na serijski monitor
  Serial.print("H: ");
  Serial.println(humidity);
  Serial.print("T: ");
  Serial.println(temperature);

  // Čitanje analogne vrijednosti sa senzora kiše
  int rainValue = analogRead(RAINPIN);

  // Ispis vrijednosti senzora kiše na serijski monitor
  Serial.print("Rain: ");
  Serial.println(rainValue);

  // Provjera pada kiše
  if (rainValue < 350) {
    padaKisa = 1;
  } else {
    padaKisa = 0;
  }

  // Ispis indikatora pada kiše na serijski monitor
  Serial.print("Pada kiša: ");
  Serial.println(padaKisa);

  // Čitanje analogne vrijednosti sa senzora vlage tla
  int soilMoisture = analogRead(SOILPIN);

  // Pretvaranje analogne vrijednosti u postotak
  soilMoisture = map(soilMoisture, 1032, 0, 0, 100);

  // Ispis vlage tla na serijski monitor
  Serial.print("Soil moisture: ");
  Serial.println(soilMoisture);

  // Čitanje UV razine
  float uvValue = AUV.read();

  // Ispis UV razine na serijski monitor
  Serial.print("UV razina ");
  Serial.println(uvValue);

  // Početak LoRa paketa
  LoRa.beginPacket();

  // Slanje podataka senzora u LoRa paketu
  LoRa.println(padaKisa);
  LoRa.println(soilMoisture);
  LoRa.println(humidity);
  LoRa.println(temperature);
  LoRa.println(uvValue);
  LoRa.endPacket();

  delay(2000);
}

float indexUV(uint16_t pin, int16_t analog_max, float voltage_max) {
  /* Ova funkcija ima za zadatak preračunati analognu vrijednost 
     pročitanu sa UV senzora u indeks UV vrijednosti.

     * Argumenti funkcije:
      - pin: broj pina na kojem je priključen UV senzor
      - analog_max: maksimalna analogna vrijednost senzora
      - voltage_max: maksimalni napon na senzoru 

     * Vrijednost koju vraća (float):  UV indeks
   */

  uint16_t raw = analogRead(pin); // Čitanje analogne vrijednosti sa senzora

  float millivolt = (voltage_max * raw * 1000) / analog_max;
  // Preračun analogne vrijednosti u napon (milivolti)

  if (millivolt < 50) { 
    // Pretpostavka da je riječ o šumu (ako je napon prenizak)
    return 0.0; // Vraća se UV indeks jednak 0
  }

  if (millivolt < 227) { 
    // Linearna interpolacija između 0 i 1 (vrijednosti UV indeksa)
    float uvi = 0.0 + (millivolt - 50.0) / (227.0 - 50.0); 
    return uvi; // Vraća izračunati UV indeks
  }

  // Linearna interpolacija između 1 i 11 (vrijednosti UV indeksa)
  // Formula je izvedena/preuzeta iz tablice vrijednosti
  float uvi = 0.0104865310 * millivolt - 1.289154988;
  return uvi; // Vraća izračunati UV indeks
} 
