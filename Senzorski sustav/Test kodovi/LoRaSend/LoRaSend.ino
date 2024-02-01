/**
 **************************************************
 *
 * @file        LoRaSender.ino
 * @brief       This example shows how to use LoRa to send packets
 *				to another LoRa module which is set as receiver
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

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 4;       // LoRa radio reset
const int irqPin = 2;         // Change for your board; must be a hardware interrupt pin

int counter = 0;

void setup()
{
  Serial.begin(9600);  //Initialize serial communication with PC
  while (!Serial);

  Serial.println("LoRa Sender");
  
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
  Serial.print("Sending packet: ");
  Serial.println(counter);

  // Send packet
  LoRa.beginPacket(); //Begin sending
  LoRa.print("hello ");	//Send payload
  LoRa.print(counter);	//Send payload
  LoRa.endPacket(); //End sending

  counter++;	//Count sended messages

  delay(5000);
}
