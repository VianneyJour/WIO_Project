#include "TFT_eSPI.h"
#include"LIS3DHTR.h"

TFT_eSPI tft;
LIS3DHTR<TwoWire> lis;


unsigned int tabPiecesNov[5][2]={{150, 150}, {2, 15}, {100, 5}, {45, 120}, {30, 20}},
tabPiecesCon[5][2]={{4, 35}, {2, 15}, {100, 5}, {45, 120}, {450, 120}},
tabPiecesExp[5][2]={{4, 35}, {2, 15}, {100, 5}, {45, 120}, {450, 120}};


unsigned int x=100;
unsigned int y=100;
const unsigned int rayonBoule = 7, rayonPiece = 5;
const unsigned int vitesse = 8;

const unsigned int xMax = 307;
const unsigned int yMax = 226;

bool start = false;


void setup() {
  tft.begin();
  tft.setRotation(3); // (0,0) à l'oppossé du joyssitck
  tft.fillScreen(TFT_DARKGREY); // fond blanc

  Serial.begin(115200);
  pinMode(WIO_BUZZER, OUTPUT);

  tft.setTextColor(TFT_BLACK);

  tft.fillCircle(x, y, rayonBoule, TFT_RED);
 
  pinMode(WIO_KEY_C, INPUT_PULLUP);
  tft.fillRect(0,0,320,40,TFT_BLUE); 
  tft.drawString("Mode 1", 10, 10); //prints strings from (10, 10)

  lis.begin(Wire1); 
  lis.setOutputDataRate(LIS3DHTR_DATARATE_25HZ); // Setting output data rage to 25Hz, can be set up tp 5kHz 
  lis.setFullScaleRange(LIS3DHTR_RANGE_2G); // Setting scale range to 2g, select from 2,4,8,16g

  // attachInterrupt(0, depart, FALLING);
}

void afficherPiece(int posTab) {
  tft.fillCircle(tabPiecesNov[posTab][0], tabPiecesNov[posTab][1],rayonPiece,TFT_YELLOW);
  tft.drawCircle(tabPiecesNov[posTab][0], tabPiecesNov[posTab][1],rayonPiece,TFT_BLACK);
  tft.drawString("$", (tabPiecesNov[posTab][0] - 2), (tabPiecesNov[posTab][1] - 3));
}

// void depart() {
//   tft.drawString("3", 100, 100); //prints strings from (10, 10)
//   delay(1000);
//   tft.drawString("2", 100, 100); //prints strings from (10, 10)
//   delay(1000);
//   tft.drawNumber("1", 100, 100); //prints strings from (10, 10)
//   delay(1000);
//   tft.drawString("GO", 100, 100); //prints strings from (10, 10)
//   analogWrite(WIO_BUZZER, 128);
//   delay(1000);
//   analogWrite(WIO_BUZZER, 0);
//   go = millis();
// }

void Giroscope() {
  float x_values, y_values ;
  x_values = lis.getAccelerationX();
  y_values = lis.getAccelerationY();

  if(y>yMax || y<(rayonBoule+40) || x>xMax || x<rayonBoule) return;

  tft.fillCircle(x, y, rayonBoule, TFT_DARKGREY);
  y = y + (x_values*vitesse);
  x = x - (y_values*vitesse);
  tft.fillCircle(x, y, rayonBoule, TFT_RED);
}

void loop() {

  if(digitalRead(WIO_KEY_C) == LOW) start = true;

  while(!start) {
    tft.drawNumber(10, 115, 115);
  }
}