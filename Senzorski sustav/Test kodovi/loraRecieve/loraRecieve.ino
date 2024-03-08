#define LORA // Definira korištenje LoRa modula za komunikaciju

#include <SPI.h> // Uključuje biblioteku za SPI komunikaciju 
#include "LoRa-SOLDERED.h" // Uključuje specifičnu LoRa biblioteku
#include <Wire.h> // Uključuje biblioteku za rad sa I2C uređajima (za OLED)
#include <Adafruit_GFX.h> // Uključuje biblioteku za rad s grafičkim sadržajem
#include <Adafruit_SSD1306.h> // Uključuje biblioteku potrebnu za rad s OLED ekranom

#define SCREEN_WIDTH 128 // Definira širinu OLED ekrana u pikselima
#define SCREEN_HEIGHT 64 // Definira visinu OLED ekrana u pikselima

// Kreira se objekt `display` klase `Adafruit_SSD1306` koji predstavlja OLED ekran
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); 

const int csPin = 10; // Definira pin za LoRa čip select
const int resetPin = 4; // Definira pin za LoRa reset
const int irqPin = 2; // Definira pin za LoRa interrupt (mora biti hardverski interrupt pin)

void setup() {
  Serial.begin(9600); // Inicijalizira serijsku komunikaciju (za ispis na računalo)
  while (!Serial); // Čeka uspostavljanje serijske komunikacije

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Pokušava inicijalizirati OLED ekran
    Serial.println(F("SSD1306 alokacija nije uspjela")); // Ispisuje poruku u slučaju neuspjeha
    for (;;); // Beskonačna petlja (ako inicijalizacija ekrana ne uspije, program staje)
  }

  delay(2000); // Čeka 2 sekunde
  display.clearDisplay(); // Briše sadržaj ekrana

  display.setTextSize(1); // Postavlja veličinu teksta
  display.setTextColor(WHITE); // Postavlja boju teksta na bijelu
  display.setCursor(0, 10); // Postavlja poziciju teksta (kursora) na ekranu

  display.println("LoRa");  // Ispisuje riječ "LoRa" na ekran
  display.println("Prijamnik"); // Ispisuje "Prijamnik" u novom redu na ekranu
  display.display();  // Prikazuje promjene na OLED ekranu

  Serial.println("LoRa prijamnik"); // Ispisuje "LoRa prijamnik" na serijski izlaz 

  LoRa.setPins(csPin, resetPin, irqPin); // Postavlja pinove za komunikaciju s LoRa modulom

  if (!LoRa.begin(868E6)) { // Pokušava inicijalizirati LoRa modul
    Serial.println("Pokretanje LoRe nije uspjelo!"); // Ispisuje poruku u slučaju neuspjeha
    while (1); // Beskonačna petlja (ako inicijalizacija ne uspije, program staje)
  }
}

void loop() {
  int packetSize = LoRa.parsePacket(); // Provjerava ima li dostupnih LoRa paketa
  display.setCursor(0, 10); // Postavlja kursor na ekranu
  display.clearDisplay(); // Briše sadržaj ekrana

  if (packetSize) { // Ako je primljen LoRa paket
    Serial.println("Paket je primljen"); // Ispisuje poruku na serijsku komunikaciju
    Serial.println("-------------------");

    // Čitaju se vrijednosti senzora iz LoRa paketa
    String padaKisa = LoRa.readStringUntil('\n'); 
    String soilMoisture = LoRa.readStringUntil('\n');
    String humidity = LoRa.readStringUntil('\n');
    String temperature = LoRa.readStringUntil('\n');
    String uvValue = LoRa.readStringUntil('\n');

    // Ispisuju se vrijednosti senzora na serijsku komunikaciju
    Serial.print("padaKisa: "); 
    Serial.println(padaKisa); 
    Serial.print("soilMoisture: ");
    Serial.println(soilMoisture);
    Serial.print("humidity: ");
    Serial.println(humidity);
    Serial.print("temperature: ");
    Serial.println(temperature);
    Serial.print("uvValue: ");
    Serial.println(uvValue);

    Serial.println("-------------------");
  }
}
