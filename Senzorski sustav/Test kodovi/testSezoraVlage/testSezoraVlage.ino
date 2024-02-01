/**
 **************************************************
 *
 * @file        LoRaReceiver.ino
 * @brief       This example shows how to use LoRa to receive packets
 *				from another LoRa module which is set as sender
 *              
 *
 *
 *
 * @authors    	Tom Igoe
 *
 * Modified by: Soldered for use with www.soldered.com/333157 , www.soldered.com/333158
 *
 ***************************************************/

///                 Arduino      RFM95/96/97/98
///                 GND----------GND   (ground in)
///                 3V3----------3.3V  (3.3V in)
///             pin D4-----------RESET  (RESET)
/// interrupt 0 pin D2-----------DIO0  (interrupt request out)
///          SS pin D10----------NSS   (CS chip select in)
///         SCK pin D13----------SCK   (SPI clock in)
///        MOSI pin D11----------MOSI  (SPI Data in)
///        MISO pin D12----------MISO  (SPI Data out)
/// This is pinout for Dasduino Core, if you are using other MCU, use SPI pins
///and Interrupt pin 0, if Dasduino ConnectPlus is used
/// (or any other ESP32 board) use pins(SS=27, RST=2, DIO0=32, MISO=33, MOSI=25,SCK=26)



#define LORA		//Specify that module will be used for LoRa to LoRa communication
#include <SPI.h>
#include "LoRa-SOLDERED.h"
#include <Wire.h>                // omogućuje komunikaciju Arduina sa I2C komponentama
#include <Adafruit_GFX.h>        // omogućuje korištenje OLED ekrana
#include <Adafruit_SSD1306.h>    // omogućuje korištenje OLED ekrana

#define SCREEN_WIDTH 128 // sirina OLED ekrana u pikselima
#define SCREEN_HEIGHT 64 // visina OLED ekrana u pikselima

// Deklariranje SSD1306 ekrana spojenog na I2C sabirnicu (SDA i SCL izvodi)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 4;       // LoRa radio reset
const int irqPin = 2;         // Change for your board; must be a hardware interrupt pin

void setup()
{
  Serial.begin(9600);  //Initialize serial communication with PC
  while (!Serial);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Adresa OLED ekrana (0x3C)
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.println("LoRa Reciever");
  display.display(); 

  Serial.println("LoRa Receiver");
  
    // Override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, IRQ pin

  if (!LoRa.begin(868E6)) // Initialize LoRa at 868 MHz
  {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop()
{
  // Try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    // Received a packet
    Serial.print("Received packet '");

    // Read packet
    while (LoRa.available())
	{
      Serial.print((char)LoRa.read()); 
    }

    // Print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }
}