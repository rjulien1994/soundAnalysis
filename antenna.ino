#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";
const byte infoSize = 9;
byte information[infoSize];

void setup() 
{
  Serial.begin(250000);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}

void loop() 
{
  Data();

  radio.write(&information, sizeof(information));
}

void Data()
{
  while(Serial.available() < infoSize)
  {
    delay(2);
  }
  
  for(byte b = 0; b < infoSize; b++)
  {
    information[b] = Serial.read();
  }
}
