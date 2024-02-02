#include <SPI.h>
#include "LoRa.h"
#include "DHT.h"  // Include DHT library

#define LORA
#define DHTPIN 5      // Define the pin where the DHT11 is connected
#define DHTTYPE DHT11 // Define the type of DHT sensor

const int csPin = 10;
const int resetPin = 4;
const int irqPin = 2;

int counter = 0;

DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Sender");

  LoRa.setPins(csPin, resetPin, irqPin);

  if (!LoRa.begin(868E6))
  {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  dht.begin();
}

void loop()
{
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  Serial.print("H: ");
  Serial.println(humidity);
  Serial.print("T: ");
  Serial.println(temperature);

  // Send packet with sensor data
  LoRa.beginPacket();
  LoRa.print("H: ");
  LoRa.println(humidity);
  LoRa.print(" T: ");
  LoRa.println(temperature);
  LoRa.endPacket();

  delay(1000);
}
