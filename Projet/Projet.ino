#include "TFT_eSPI.h"
#include "LIS3DHTR.h"
#include <math.h>

TFT_eSPI tft;
LIS3DHTR<TwoWire> lis;

unsigned int  tabPiecesNov[5][2]={{170, 170}, {160, 105}, {100, 205}, {145, 120}, {130, 220}}, // tableau des pieces du mode novice
              tabPiecesCon[5][2]={{4, 100}, {2, 46}, {100, 80}, {45, 120}, {250, 120}}, // tableau des pieces du mode confirmé
              tabPiecesExp[5][2]={{4, 138}, {2, 54}, {100, 69}, {45, 120}, {250, 120}}, //tableau des pieces du mode expert
              tabPiecesNoires[5][2]={{60, 138}, {50, 54}, {100, 90}, {45,200}, {250, 20}}, //tableau des pieces du mode expert
              tabActuel[5][2]; //tableau du mode actuel

const unsigned int xMax = 307, yMax = 229;
unsigned int xBoule=xMax/2, yBoule=yMax/2; // position x et y de départ de la boule
const unsigned int rayonBoule = 7, rayonPiece = 5, rayonPieceNoire = 5; // rayon de la boule et des pieces
const unsigned int vitesseBoule = 8; // vitesse de déplacement de la boule

unsigned int mode = 2; 

int chrono = 30000, score = 0;
bool start = false;

void setup() {
  tft.begin();
  tft.setRotation(3); // (0,0) à l'oppossé du joyssitck
  Serial.begin(115200);

  // Mise en place des éléments sur l'écran :
  tft.fillScreen(TFT_DARKGREY); // fond gris foncé
  tft.fillCircle(xBoule, yBoule, rayonBoule, TFT_RED); // Dessin de la boule en rouge
  tft.fillRect(0,0,320,40,TFT_BLUE);  // Rectangle du bandeau de scores en bleu

  // Paramétrage du giroscope :
  lis.begin(Wire1); 
  lis.setOutputDataRate(LIS3DHTR_DATARATE_25HZ); // Setting output data rage to 25Hz, can be set up tp 5kHz 
  lis.setFullScaleRange(LIS3DHTR_RANGE_2G); // Setting scale range to 2g, select from 2,4,8,16g
  tft.setTextColor(TFT_BLACK); // couleur du text en noir
  tft.setTextSize(2);
  tft.drawString(String(score)+"pts", 150, 10);


  // Déclaration des capteurs :
  pinMode(WIO_KEY_C, INPUT_PULLUP); // Déclaration du bouton C en INPUT
  pinMode(WIO_KEY_B, INPUT_PULLUP);
  pinMode(WIO_BUZZER, OUTPUT); // Déclaration du buzzer en OUTPUT
}

void depart() {
  // if(go == 3000) {
  //   analogWrite(WIO_BUZZER, 128);
  // }
  // if(go == 3500) {
  //   analogWrite(WIO_BUZZER, 0);
  // }
  // go = millis();
}


// fonction d'affichage des pieces
void afficherPiece(unsigned int tab[5][2]) {
  for(int i=0; i<5; i++) {
    tft.fillCircle(tab[i][0], tab[i][1],rayonPiece,TFT_YELLOW); // Dessin du cercle plein en jaune de la piece
    tft.drawCircle(tab[i][0], tab[i][1],rayonPiece,TFT_BLACK); // Contour noir sur la piece
    tft.setTextSize(1);
    tft.drawString("$", (tab[i][0] - 2), (tab[i][1] - 3)); // Icon dollar sur le centre de la piece
    tft.setTextSize(2);
  }
}

// fonction d'affichage des pieces
void afficherPiecesNoires(unsigned int tab[5][2]) {
  for(int i=0; i<5; i++) {
    tft.fillCircle(tab[i][0], tab[i][1],rayonPieceNoire,TFT_BLACK); // Dessin du cercle plein en jaune de la piece
  }
}

bool colisions(int xPiece, int yPiece) {
  double distance = sqrt(sq(xPiece - xBoule) + sq(yPiece - yBoule));
  if(distance <= (rayonBoule + rayonPiece)) {
    return true;
  }
  return false;
}

void Gyroscope() {
  float x_values, y_values ;
  x_values = lis.getAccelerationX();
  y_values = lis.getAccelerationY();

  if(yBoule>yMax || yBoule<(rayonBoule+43) || xBoule>xMax || xBoule<(rayonBoule+2)) {
    start = false;
    tft.fillCircle(xBoule, yBoule, rayonBoule, TFT_DARKGREY);
    score = 0;
    tft.fillRect(150, 10, 60, 25, TFT_BLUE);
    tft.drawString(String(score)+"pts", 150, 10);
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

void copie_tableau(unsigned int tab[5][2])
{
  for(int i=0; i<5; i++) {
    tabActuel[i][0] = tab[i][0];
    tabActuel[i][1] = tab[i][1];
  }
}

bool victoire(unsigned int tab[5][2])
{
  for(int i=0; i<5; i++)
    if(tab[i][0]!=1000 || tab[i][1]!=1000) return false;
  return true;
}

void menu() {

  if(digitalRead(WIO_KEY_B) == LOW) {
    start = true;
    score = 0;
    tft.fillRect(150, 10, 60, 25, TFT_BLUE);
    tft.drawString(String(score)+"pts", 150, 10);

    if(mode == 0) {
      copie_tableau(tabPiecesNov);
    }
    if(mode == 1) {
      copie_tableau(tabPiecesCon);
    }
    if(mode == 2) {
      copie_tableau(tabPiecesExp);
    }
  }

  if(digitalRead(WIO_KEY_C) == LOW) {
    mode = (mode + 1)%3;

    tft.fillRect(0,0,150,40,TFT_BLUE);  // Rectangle du bandeau de scores en bleu
    tft.fillRect(0,40,320,226,TFT_DARKGREY);  // Rectangle du fond en gris

    if(mode == 0) {
      tft.drawString("novice", 10, 10); // affiche le texte en position (10, 10)
      copie_tableau(tabPiecesNov);
      afficherPiece(tabActuel);
    }
    if(mode == 1) {
      tft.drawString("confirme", 10, 10); // affiche le texte en position (10, 10)
      copie_tableau(tabPiecesCon);
      afficherPiece(tabActuel);
    }
    if(mode == 2) {
      tft.drawString("expert", 10, 10); // affiche le texte en position (10, 10)
      copie_tableau(tabPiecesExp);
      afficherPiece(tabActuel);
      afficherPiecesNoires(tabPiecesNoires);
    }

    start = false;
    delay(100);
  }
}

void loop() {
  //lancer un mode avant de lancer la game, c'est mieux

  if(start) {
    if(chronoCalcule()) {
      Gyroscope();
      
      for(int i =0 ; i< 5; i++)
      {
        if(colisions(tabActuel[i][0], tabActuel[i][1]))
        {
          tft.fillCircle(tabActuel[i][0], tabActuel[i][1],rayonPiece,TFT_DARKGREY); // Dessin du cercle plein en gris pour cacher la piece
          score = score + 10;
          // analogWrite(WIO_BUZZER, 128);
          tft.fillRect(150, 10, 60, 25, TFT_BLUE);
          tft.drawString(String(score)+"pts", 150, 10);
          tabActuel[i][0] = 1000;
          tabActuel[i][1] = 1000;
        }
        if(colisions(tabPiecesNoires[i][0], tabPiecesNoires[i][1]))
        {
          tft.fillCircle(tabPiecesNoires[i][0], tabPiecesNoires[i][1],rayonPieceNoire,TFT_DARKGREY); // Dessin du cercle plein en gris pour cacher la piece
          chrono = chrono - 10000;
          // analogWrite(WIO_BUZZER, 128);
          tabPiecesNoires[i][0] = 1000;
          tabPiecesNoires[i][1] = 1000;
        }
      }

      if(victoire(tabActuel))
      {
        start = false;
        score = score + chrono/1000;
        tft.fillRect(150, 10, 60, 25, TFT_BLUE);
        tft.drawString(String(score)+"pts", 150, 10);
      }
    }
    else start = false;
  }
  else{
    menu();
    chrono = 30000;
    xBoule=100;
    yBoule=100;
  }

  delay(10);
}