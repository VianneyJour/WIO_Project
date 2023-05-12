#include "TFT_eSPI.h"
#include "LIS3DHTR.h"

TFT_eSPI tft;
LIS3DHTR<TwoWire> lis;


unsigned int  tabPiecesNov[5][2]={{150, 150}, {2, 15}, {100, 5}, {45, 120}, {30, 20}}, // tableau des pieces du mode novice
              tabPiecesCon[5][2]={{4, 35}, {2, 15}, {100, 5}, {45, 120}, {450, 120}}, // tableau des pieces du mode confirmé
              tabPiecesExp[5][2]={{4, 35}, {2, 15}, {100, 5}, {45, 120}, {450, 120}}; // tableau des pieces du mode expert


unsigned int xBoule=100; // position x de départ de la boule
unsigned int yBoule=100; // position y de départ de la boule
const unsigned int rayonBoule = 7, rayonPiece = 5; // rayon de la boule et des pieces
const unsigned int vitesseBoule = 8; // vitesse de déplacement de la boule

enum mode {novice, confirme, expert};

const unsigned int xMax = 307; 
const unsigned int yMax = 226;

bool start = false;


void setup() {
  tft.begin();
  tft.setRotation(3); // (0,0) à l'oppossé du joyssitck
  Serial.begin(115200);

  // Mise en place des éléments sur l'écran :
  tft.fillScreen(TFT_DARKGREY); // fond gris foncé
  tft.fillCircle(xBoule, yBoule, rayonBoule, TFT_RED); // Dessin de la boule en rouge
  tft.fillRect(0,0,320,40,TFT_BLUE);  // Rectangle du bandeau de scores en bleu
  tft.setTextColor(TFT_BLACK); // couleur du text en noir
  tft.drawString("Mode novice", 10, 10); // affiche le texte en position (10, 10)

  // Paramétrage du giroscope :
  lis.begin(Wire1); 
  lis.setOutputDataRate(LIS3DHTR_DATARATE_25HZ); // Setting output data rage to 25Hz, can be set up tp 5kHz 
  lis.setFullScaleRange(LIS3DHTR_RANGE_2G); // Setting scale range to 2g, select from 2,4,8,16g

  // Déclaration des capteurs :
  pinMode(WIO_KEY_C, INPUT_PULLUP); // Déclaration du bouton C en INPUT
  pinMode(WIO_BUZZER, OUTPUT); // Déclaration du buzzer en OUTPUT
}

// fonction d'affichage d'une piece
void afficherPiece(int posTab) {
  tft.fillCircle(tabPiecesNov[posTab][0], tabPiecesNov[posTab][1],rayonPiece,TFT_YELLOW); // Dessin du cercle plein en jaune de la piece
  tft.drawCircle(tabPiecesNov[posTab][0], tabPiecesNov[posTab][1],rayonPiece,TFT_BLACK); // Contour noir sur la piece
  tft.drawString("$", (tabPiecesNov[posTab][0] - 2), (tabPiecesNov[posTab][1] - 3)); // Icon dollar sur le centre de la piece
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

  if(yBoule>yMax || yBoule<(rayonBoule+40) || xBoule>xMax || xBoule<rayonBoule) return;

  tft.fillCircle(xBoule, yBoule, rayonBoule, TFT_DARKGREY);
  yBoule = yBoule + (x_values * vitesseBoule);
  xBoule = xBoule - (y_values * vitesseBoule);
  tft.fillCircle(xBoule, yBoule, rayonBoule, TFT_RED);
}

void loop() {

  if(digitalRead(WIO_KEY_C) == LOW) {
    start = true;
  }
  if(digitalRead(WIO_KEY_B) == LOW){
    tft.fillRect(10,10,50,50,TFT_BLUE);  // Rectangle du bandeau de scores en bleu
    // mode = mode + 1;
    // tft.drawString("Mode " + mode, 10, 10);
  }

  while(start == false) {
    
  }
}