#include <AnalogUVSensor.h>
#include <SPI.h>
#include "LoRa.h"
#include "DHT.h"  // Include DHT library

#define LORA
#define DHTTYPE DHT11  // Define the type of DHT sensor

#define RAINPIN A0
#define SOILPIN A1
#define DHTPIN A2
#define UVPIN A3

const int csPin = 10;
const int resetPin = 4;
const int irqPin = 2;

int counter = 0;
int padaKisa = 0;

AnalogUVSensor AUV;
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  pinMode(RAINPIN, INPUT);
  pinMode(SOILPIN, INPUT);
  pinMode(DHTPIN, INPUT);
  pinMode(UVPIN, INPUT);
  while (!Serial)
    ;

  Serial.println("LoRa Sender");

  LoRa.setPins(csPin, resetPin, irqPin);

  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }
  AUV.begin(UVPIN, 3.3, 1023);
  dht.begin();
}

void loop() {
  //dht sensor
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  Serial.print("H: ");
  Serial.println(humidity);
  Serial.print("T: ");
  Serial.println(temperature);

  // rain sensor
  int rainValue = analogRead(RAINPIN);
  Serial.print("Rain: ");
  Serial.println(rainValue);
  if (rainValue < 350) {
    padaKisa = 1;
  } else {
    padaKisa = 0;
  }
  Serial.print("Pada kiša: ");
  Serial.println(padaKisa);

  // soil sensor
  int soilMoisture = analogRead(SOILPIN);
  Serial.print("Soil moisture: ");
  soilMoisture = map (soilMoisture, 1032, 0, 0, 100);
  Serial.println(soilMoisture);


  // UV sensor
  float uvValue = AUV.read();
  Serial.print("UV razina ");
  Serial.println(uvValue);

  // Send packet with sensor data
  LoRa.beginPacket();
  LoRa.println(padaKisa);
  LoRa.println(soilMoisture);
  LoRa.println(humidity);
  LoRa.println(temperature);
  LoRa.println(uvValue);
  LoRa.endPacket();

  delay(2000);
}

float indexUV(uint16_t pin, int16_t analog_max, float voltage_max) {
  uint16_t raw = analogRead(pin);
  float millivolt = (voltage_max * raw * 1000) / analog_max;

  if (millivolt < 50) {
    //  assume noise.
    return 0.0;
  }
  if (millivolt < 227) {
    //  linear interpolation between 0..1
    float uvi = 0.0 + (millivolt - 50.0) / (227.0 - 50.0);
    return uvi;
  }
  //  linear interpolation between 1..11
  //  formula derived with spreadsheet.
  float uvi = 0.0104865310 * millivolt - 1.289154988;
  return uvi;
}
