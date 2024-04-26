/*
** Autor: Luiz H. Cassettari**
** Datum: 05.08.2018  **

** Prilagodba koda: Centar Pozitron
** Projekt: AIAgro**

** Opis: 
** Ovaj kod demonstrira stvaranje jednostavne LoRa Gateway/Čvor logike 
** koristeći InvertIQ funkcionalnost. Implementira jednosmjerni način 
** komunikacije gdje Gateway šalje, a Čvor prima.

** Funkcionalnost: **
* **Gateway:**
    * Šalje poruke s omogućenim InvertIQ
    * Prima poruke s onemogućenim InvertIQ
* **Čvor:**
    * Šalje poruke s onemogućenim InvertIQ
    * Prima poruke s omogućenim InvertIQ

** Posebnost: InvertIQ funkcija invertira LoRa I i Q signale, što sprječava
** komunikaciju između više Gateway-eva ili više Čvorova, osiguravajući 
** ekskluzivnost komunikacije Gateway-Čvor.
*/

#include <SPI.h> // Uključuje SPI knjižnicu
#include <LoRa.h> // Uključuje LoRa knjižnicu

const long frequency = 868E6; // Frekvencija LoRa modula

const int csPin = 10;    // Pin za selekciju LoRa modula
const int resetPin = 4;  // Pin za resetiranje LoRa modula 
const int irqPin = 2;    // Pin za prekid (mora biti hardverski pin za prekid)

void setup() {
  Serial.begin(9600);  // Inicijalizacija serijske komunikacije
  while (!Serial);     // Čekaj na uspostavu serijske veze 

  LoRa.setPins(csPin, resetPin, irqPin); // Postavljanje pinova za LoRa modul

  if (!LoRa.begin(frequency)) {  // Pokušaj inicijalizacije LoRa modula
    Serial.println("LoRa inicijalizacija nije uspjela. Provjerite veze."); 
    while (true);  // U slučaju greške, pauziraj izvođenje programa
  }

  LoRa.onReceive(onReceive);  // Postavljanje callback funkcije za primanje poruka
  LoRa_rxMode();              // Postavljanje LoRa modula u režim prijema
}

void loop() {
  // Glavna petlja programa - ne izvodi se ništa osim callback funkcije
}

void LoRa_rxMode() {
  LoRa.disableInvertIQ(); // Postavljanje normalnog režima rada
  LoRa.receive();         // Postavljanje LoRa modula u režim prijema
}

void onReceive(int packetSize) {
  String message = "";   // Varijabla za spremanje primljene poruke

  while (LoRa.available()) {   // Sve dok ima dostupnih podataka
    message += (char)LoRa.read();  // Pročitaj znak i dodaj ga u poruku
  }

  Serial.println(message);   // Ispis primljene poruke na serijski port - slanje na esp32 modul
}
