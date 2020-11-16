#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

const byte PIN = 2;
//const byte PIR = 3;
//const byte echo = 4;
//const byte trig = 5;
RF24 radio(7, 8); // CE, CSN


Adafruit_NeoPixel strip = Adafruit_NeoPixel(150, PIN, NEO_GRB + NEO_KHZ800);
byte pixelsID[15][10];

const byte address[6] = "00001";

const byte color[15][3] = 
{
  {128,0,0},  {230,25,75},  {250,190,190},{170,110,40}, {245,130,48},
  {128,128,0},{255,255,25}, {210,245,60}, {60,180,75},  {70,240,240},
  {0,0,128},  {0,130,200},  {145,30,180}, {240,50,230}, {128,128,128}
};
const byte maroon = 0; const byte red = 1; const byte pink = 2; const byte brown = 3; const byte orange = 4;
const byte olive = 5; const byte yellow = 6; const byte lime = 7; const byte green = 8; const byte cyan = 9;
const byte navy = 10; const byte blue = 11; const byte purple = 12; const byte magnenta = 13; const byte grey = 14;

const byte letters[52][5] =
{
  {96, 148, 252, 0, 0}, {255, 136, 112, 0, 0},  {112, 136, 136, 0, 0},      {112, 136, 255, 0, 0},  {248, 164, 152, 0, 0},
  {254, 9, 1, 0, 0},    {152, 164, 248, 0, 0},  {255, 16,  224, 0, 0},      {250, 0, 0, 0, 0},      {128, 122, 0, 0, 0},
  {255, 40, 208, 0, 0}, {255, 128, 0, 0, 0},    {240, 8, 240, 8, 240},      {240, 8, 240, 0, 0},    {112, 136, 112, 0, 0},
  {252, 20, 8, 0, 0},   {8, 20, 252, 0, 0},     {240, 8, 0, 0, 0},          {144, 168, 72,  0, 0},  {126, 132, 0, 0, 0},
  {240, 128, 240, 0, 0},{112, 128, 112, 0, 0},  {120, 128, 96,  128, 120},  {216, 32, 216, 0, 0},   {184, 160, 248, 0, 0},
  {200, 168, 152, 0, 0},
  {254, 9, 9, 254, 0},    {255, 137, 118, 0, 0},    {126, 129, 129, 0, 0},    {255, 129, 126, 0, 0},  {255, 137, 129, 0, 0},
  {255, 9, 1, 0, 0},      {126, 129, 145, 113, 0},  {255, 16, 16, 255, 0},    {129, 255, 129, 0, 0},  {129, 127, 1, 0, 0},
  {255, 16,  40, 199, 0}, {255, 128, 128, 0, 0},    {254, 1, 6, 1, 254},      {255, 4, 8, 255, 0},    {126, 129, 129, 126, 0},
  {255, 17, 14, 0, 0},    {126, 129, 65, 190, 0},   {255, 9, 246, 0, 0},      {134, 137, 113, 0, 0},  {1, 255, 1, 0, 0},
  {127, 128, 128, 127, 0},{31, 96, 128, 96, 31},    {127, 128, 112, 128, 127},{195, 52, 8, 52, 195},  {3, 4, 248, 4, 3},
  {225, 145, 137, 133, 131}
  
};
const byte a = 0, b = 1,  c = 2,  d = 3,  e = 4,  f = 5,  g = 6,  h = 7,  i = 8,  j = 9,  k = 10, l = 11, m = 12, n = 13, o = 14, p = 15, q = 16, r = 17, s = 18, t = 19, u = 20, v = 21, w = 22, x = 23, y = 24, z = 25;
const byte A = 26,B = 27, C = 28, D = 29, E = 30, F = 31, G = 32, H = 33, I = 34, J = 35, K = 36, L = 37, M = 38, N = 39, O = 40, P = 41, Q = 42, R = 43, S = 44, T = 45, U = 46, V = 47, W = 48, X = 49, Y = 50, Z = 51;

const byte sizeInfo = 9;
byte information[sizeInfo] = {1,1,1,1,1,1,1,1,1};
byte Max[sizeInfo] = {1,1,1,1,1,1,1,1,1};
byte freqCap[8] = {0,0,0,0,0,0,0,0};
unsigned long timerOne = 10000;
unsigned long timerTwo = 10000;
//boolean presence = 0;
byte program = 0;

void setup() 
{
  //pinMode(PIR, INPUT);    //Mvt sensor set as input
  //pinMode(echo, INPUT);   //ultrasonic sonic set up
  //pinMode(trig, OUTPUT);
  
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  strip.begin();
  strip.setBrightness(50);
  strip.show();
  screenID();
}

void loop() 
{
  collectData();
  freqVizual();
  
  if(information[1] == 0 && information[4] == 0)
  {
    clearScreen();
    while(information[1] == 0)
    {
      collectData();
    }
  }
}

void freqVizual()
{
  for(byte frequency = 0; frequency < sizeInfo; frequency++)
  {
    if(information[frequency] >  Max[frequency])
    {
      Max[frequency] = information[frequency];
    }
  }
  
  byte adjVolume = min(100,max(adjVolume - 1, byte(float(100*information[sizeInfo-1])/Max[sizeInfo-1])));  //ratio to max volume
  adjVolume = max(20, adjVolume);
  
  strip.setBrightness(adjVolume);
  
  byte adjFreq;
  
  for(byte frequency = 0; frequency < 8; frequency++)
  {
    adjFreq = min(10,byte(float(10*information[frequency])/Max[frequency]));       //ratio to max freq
    freqCap[frequency] = max(freqCap[frequency],adjFreq);
    
    for(byte horizontal = 2*frequency; horizontal < min(15,2*(frequency+1)); horizontal++)
    {
      for(byte vertical = 0; vertical < 10; vertical++)
      {
        if(vertical == freqCap[frequency])
        {
          strip.setPixelColor(pixelsID[horizontal][vertical], 0, 0, 150);
        }
        else if(vertical <= adjFreq)
        {
          strip.setPixelColor(pixelsID[horizontal][vertical], color[horizontal][0], color[horizontal][1], color[horizontal][2]);
        }
        else
        {
          strip.setPixelColor(pixelsID[horizontal][vertical], 0, 0, 0);
        }
      }
    }
  }
  strip.show();
  
  if(millis() - 50 > timerOne)
  {
    timerOne = millis();
    for(byte frequency = 0; frequency < 8; frequency++)
    {
      freqCap[frequency] = max(0,freqCap[frequency]-1);
    }
  }
  if(millis() - 10000 > timerTwo)
  {
    timerTwo = millis();
    for(byte frequency = 0; frequency < 8; frequency++)
    {
      Max[frequency] = max(5,Max[frequency]-1);
    }
  }
}

void collectData()
{
  if(radio.available())
  {
    radio.read(&information, sizeof(information));
  }
}

void clearScreen()
{
  for(byte leds = 0; leds < 150; leds++)
  {
    strip.setPixelColor(leds, 0, 0, 0);
  }
  strip.show();
}

void screenID()   //organize pixels id in a (x,y) matrix
{
  for(byte vertical = 0; vertical < 10; vertical++)
  {
    if(vertical%2 == 0)
    {
      for(byte horizontal = 0; horizontal < 15; horizontal++)
      {
        pixelsID[horizontal][vertical] = (vertical+1)*15 - (horizontal+1);
      }
    }
    else
    {
      for(byte horizontal = 0; horizontal < 15; horizontal++)
      {
        pixelsID[horizontal][vertical] = vertical*15 + horizontal;
      }
    }
  }
}
