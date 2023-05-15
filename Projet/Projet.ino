#include "TFT_eSPI.h"
#include "LIS3DHTR.h"

TFT_eSPI tft;
LIS3DHTR<TwoWire> lis;

unsigned int  tabPiecesNov[5][2]={{170, 170}, {160, 105}, {100, 205}, {145, 120}, {130, 220}}, // tableau des pieces du mode novice
              tabPiecesCon[5][2]={{4, 100}, {2, 46}, {100, 80}, {45, 120}, {250, 120}}, // tableau des pieces du mode confirmé
              tabPiecesExp[5][2]={{4, 138}, {2, 54}, {100, 69}, {45, 120}, {250, 120}}; // tableau des pieces du mode expert



unsigned int xBoule=100, yBoule=100; // position x et y de départ de la boule
const unsigned int rayonBoule = 7, rayonPiece = 5; // rayon de la boule et des pieces
const unsigned int vitesseBoule = 8; // vitesse de déplacement de la boule


unsigned int mode = 0; 

const unsigned int yMax = 226, xMax = 307; // gerer le bord du bandeau
int chrono = 30000;
bool time, start = false;



void setup() {
  tft.begin();
  tft.setRotation(3); // (0,0) à l'oppossé du joyssitck
  Serial.begin(115200);

  // Mise en place des éléments sur l'écran :
  tft.fillScreen(TFT_DARKGREY); // fond gris foncé
  tft.fillCircle(xBoule, yBoule, rayonBoule, TFT_RED); // Dessin de la boule en rouge
  tft.fillRect(0,0,320,40,TFT_BLUE);  // Rectangle du bandeau de scores en bleu
  tft.setTextColor(TFT_BLACK); // couleur du text en noir

  // Paramétrage du giroscope :
  lis.begin(Wire1); 
  lis.setOutputDataRate(LIS3DHTR_DATARATE_25HZ); // Setting output data rage to 25Hz, can be set up tp 5kHz 
  lis.setFullScaleRange(LIS3DHTR_RANGE_2G); // Setting scale range to 2g, select from 2,4,8,16g


  // Déclaration des capteurs :
  pinMode(WIO_KEY_C, INPUT_PULLUP); // Déclaration du bouton C en INPUT
  pinMode(WIO_KEY_B, INPUT_PULLUP);
  pinMode(WIO_BUZZER, OUTPUT); // Déclaration du buzzer en OUTPUT
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


// fonction d'affichage d'une piece
void afficherPiece(unsigned int tab[5][2]) {
  for(int i=0; i<4; i++) {
    tft.fillCircle(tab[i][0], tab[i][1],rayonPiece,TFT_YELLOW); // Dessin du cercle plein en jaune de la piece
    tft.drawCircle(tab[i][0], tab[i][1],rayonPiece,TFT_BLACK); // Contour noir sur la piece
    tft.drawString("$", (tab[i][0] - 2), (tab[i][1] - 3)); // Icon dollar sur le centre de la piece
  }
}

bool colisions(int xPiece, int yPiece) {
  //distance = sqrt((xBoule - xPiece)^^2 + (yBoule - yPiece)^^2);
  //if(distance == 0) return true;
  return false;
}

void Gyroscope() {
  float x_values, y_values ;
  x_values = lis.getAccelerationX();
  y_values = lis.getAccelerationY();

  if(yBoule>yMax || yBoule<(rayonBoule+40) || xBoule>xMax || xBoule<rayonBoule) {
    start = false;
    tft.fillCircle(xBoule, yBoule, rayonBoule, TFT_DARKGREY);
    return;
  }
  tft.fillCircle(xBoule, yBoule, rayonBoule, TFT_DARKGREY);
  yBoule = yBoule + (x_values * vitesseBoule);
  xBoule = xBoule - (y_values * vitesseBoule);
  tft.fillCircle(xBoule, yBoule, rayonBoule, TFT_RED);
}

bool chronoCalcule() {
  if(chrono < 1) {
    start = false;
    return false;
  }
  chrono = chrono -10;
  if(chrono/1000 < (chrono+10)/1000)
  {
    tft.fillRect(250,10,30,20,TFT_BLUE);
    tft.drawNumber(chrono/1000, 250, 10);
  }
  return true;
}

void menu() {
  if(digitalRead(WIO_KEY_B) == LOW) {
    start = true;
  }
  if(digitalRead(WIO_KEY_C) == LOW) {
    mode = (mode + 1)%3;
    if(mode == 0) {
      tft.fillRect(0,0,150,40,TFT_BLUE);  // Rectangle du bandeau de scores en bleu
      tft.drawString("novice", 10, 10); // affiche le texte en position (10, 10)
      tft.fillRect(0,40,320,226,TFT_DARKGREY);  // Rectangle du bandeau de scores en bleu
      afficherPiece(tabPiecesNov);
    }
    if(mode == 1) {
      tft.fillRect(0,0,150,40,TFT_BLUE);  // Rectangle du bandeau de scores en bleu
      tft.drawString("confirme", 10, 10); // affiche le texte en position (10, 10)
      tft.fillRect(0,40,320,226,TFT_DARKGREY);  // Rectangle du bandeau de scores en bleu
      afficherPiece(tabPiecesCon);
    }
    if(mode == 2) {
      tft.fillRect(0,0,150,40,TFT_BLUE);  // Rectangle du bandeau de scores en bleu
      tft.drawString("expert", 10, 10); // affiche le texte en position (10, 10)
      tft.fillRect(0,40,320,226,TFT_DARKGREY);  // Rectangle du bandeau de scores en bleu
      afficherPiece(tabPiecesExp);
    }
    start = false;
    delay(100);
  }
}

void loop() {

  menu();
  if(start) {
    time = chronoCalcule();
    if(time) {
      Gyroscope();
      
    //  for(int i =0 ; i< 4; i++)
    //   if(colision());
    }
  }
  else{
    chrono = 30000;
    xBoule=100;
    yBoule=100;
  }

  delay(10);
}