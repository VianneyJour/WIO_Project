/* Authors
    Vianney Jourdy
    Antoine Perederii
*/

#include "TFT_eSPI.h"
#include "LIS3DHTR.h"
#include <math.h>

TFT_eSPI tft;
LIS3DHTR<TwoWire> lis;

#define fondEcran TFT_DARKGREY // Declaration de la couleur du fond d'ecran
#define couleurBandeau TFT_BLUE // Declaration de la couleur du bandeau
#define couleurBoule TFT_RED // Declaration de la couleur de la boule
#define couleurPieces TFT_YELLOW // Declaration de la couleur des pieces
#define couleurPiecesNoires TFT_BLACK // Declaration de la couleur des pieces noires
#define couleurPolice TFT_BLACK // Declaration de la couleur de la police d'ecriture

const unsigned int nbPiecesTab = 5, nbPiecesNoiresTab = 5;

// Déclaration des trois tableau de pieces, du tableau de boules noires et des deux tableau que nous utiliseronsdans le programme
unsigned int  tabPiecesNov[nbPiecesTab][2]={{70, 70}, {260, 75}, {89, 205}, {220, 120}, {259, 200}}, // tableau des pieces du mode novice
              tabPiecesCon[nbPiecesTab][2]={{15, 220}, {280, 54}, {100, 69}, {45, 120}, {250, 120}}, // tableau des pieces du mode confirmé
              tabPiecesExp[nbPiecesTab][2]={{15, 220}, {280, 54}, {100, 69}, {45, 120}, {250, 120}}, //tableau des pieces du mode expert
              tabPiecesNoires[nbPiecesNoiresTab][2]={{250, 138}, {50, 70}, {250, 220}, {45,180}, {250, 75}}, //tableau des pieces noires du mode expert
              tabActuel[nbPiecesTab][2], // tableau du mode actuel
              tabActuelNoir[nbPiecesNoiresTab][2]; //tableau de pieces noires du mode actuel

const unsigned int xMax = 307, yMax = 229; // valeurs maximales de l'écran
unsigned int xBoule=xMax/2, yBoule=yMax/2; // position x et y de départ de la boule
const unsigned int rayonBoule = 7, rayonPiece = 5, rayonPieceNoire = 5; // rayon de la boule, des pieces et des pieces noires
const unsigned int vitesseBoule = 13; // vitesse de déplacement de la boule

int mode = -1; // declaration de la variable permettant de gerer le mode de jeu (novice, confirme et expert) 

int chrono = 30000, score = 0; // declaration de la variable de temps de jeu (chrono) et du score initialise a 0
unsigned int i; // iterateur pour les boucles for
bool start = false, bonus=true; // declaration des booleen de depart du jeu et du bouton bonus de temps

void setup() {
  tft.begin();
  tft.setRotation(3); // (0,0) à l'oppossé du joyssitck
  Serial.begin(115200);

  // Mise en place des éléments sur l'écran :
  tft.fillScreen(fondEcran); // fond gris foncé
  tft.fillCircle(xBoule, yBoule, rayonBoule, couleurBoule); // Dessin de la boule en rouge
  tft.fillRect(0,0,320,40,couleurBandeau);  // Rectangle du bandeau de scores en bleu

  // Paramétrage du giroscope :
  lis.begin(Wire1); 
  lis.setOutputDataRate(LIS3DHTR_DATARATE_25HZ); // Setting output data rage to 25Hz, can be set up tp 5kHz 
  lis.setFullScaleRange(LIS3DHTR_RANGE_2G); // Setting scale range to 2g, select from 2,4,8,16g
  tft.setTextColor(TFT_BLACK); // couleur du text en noir
  tft.setTextSize(2); // definission de la taille du text en 2
  tft.drawString("Bienvenue dans notre jeu", 15, 10); // affichage du mot de bienvenue dans le jeu
  delay(2000); // delay permettant d'afficher le mot de bienvenue
  tft.fillRect(0,0,320,40,couleurBandeau);  // Rectangle du bandeau permettant d'effacer le mot de bienvenue
  tft.drawString("A votre tour :", 80, 10); // message de préparation au jeu
  delay(2000);// delay permettant d'afficher le mot de preparation
  tft.fillRect(0,0,320,40,couleurBandeau);  // Rectangle du bandeau permettant d'effacer le mot de preparation

  // Déclaration des capteurs :
  pinMode(WIO_KEY_A, INPUT_PULLUP); // Déclaration du bouton A en INPUT
  pinMode(WIO_KEY_B, INPUT_PULLUP); // Déclaration du bouton B en INPUT
  pinMode(WIO_KEY_C, INPUT_PULLUP); // Déclaration du bouton C en INPUT
  pinMode(WIO_BUZZER, OUTPUT); // Déclaration du buzzer en OUTPUT
}

// fonction du son de depart de jeu
void depart() {
  tone(WIO_BUZZER, 1500, 1000); // son de depart
  delay(1000); // delay d'une seconde
}

// fonction d'affichage des pieces
void afficherPiece(unsigned int tab[nbPiecesTab][2]) {
  for(i=0; i<nbPiecesTab; i++) { // parcours des pieces du tableau
    tft.fillCircle(tab[i][0], tab[i][1],rayonPiece,couleurPieces); // Dessin du cercle plein en jaune de la piece
    tft.drawCircle(tab[i][0], tab[i][1],rayonPiece,couleurPolice); // Contour noir sur la piece
    tft.setTextSize(1); // definission d'une petite taille d'affichage du dollar
    tft.drawString("$", (tab[i][0] - 2), (tab[i][1] - 3)); // Icon dollar sur le centre de la piece
    tft.setTextSize(2); // redefinission de la taille de base d'affichage
  }
}

// fonction d'affichage des pieces noires
void afficherPiecesNoires(unsigned int tab[nbPiecesNoiresTab][2]) {
  for(i=0; i<nbPiecesNoiresTab; i++) { // parcours du tableau des pieces noires
    tft.fillCircle(tab[i][0], tab[i][1],rayonPieceNoire,couleurPiecesNoires); // Dessin du cercle plein en noir
  }
}

// fonction de collision des pieces avec la boule rouge
bool colisions(int xPiece, int yPiece) { // passage en parametre des coordonées des pieces et pieces noires
  double distance = sqrt(sq(xPiece - xBoule) + sq(yPiece - yBoule)); // calcule des distances entre chaque pieces de la boule
  if(distance <= (rayonBoule + rayonPiece)) { // Si la distance est inférieur au rayon des boules alors :
    return true; // retourner vraie
  }
  return false; // sinon retourner faux
}

// fonction du gyroscope
void Gyroscope() {
  float x_values, y_values ;
  x_values = lis.getAccelerationX();
  y_values = lis.getAccelerationY();

  if(yBoule>yMax || yBoule<(rayonBoule+44) || xBoule>xMax || xBoule<(rayonBoule+2)) { // si la boule sort du cadre de l'ecran alors :
    start = false; // on arrete le jeu
    tft.fillCircle(xBoule, yBoule, rayonBoule, fondEcran); // On efface la boule rouge
    score = 0; // on definie le score a 0
    tft.fillRect(150, 10, 60, 25, couleurBandeau); // on actualise le score a l'ecran
    tft.drawString(String(score)+"pts", 150, 10); // on actualise le score a l'ecran
    tone(WIO_BUZZER, 450, 300); // son de défaite
    delay(300); // son de défaite
    tone(WIO_BUZZER, 350, 300); // son de défaite
    delay(300); // son de défaite
    tone(WIO_BUZZER, 250, 300); // son de défaite
    delay(300); // son de défaite
    return; // sort de la fonction
  }
  tft.fillCircle(xBoule, yBoule, rayonBoule, fondEcran); // sinon on efface la boule pour actualiser ses coordonnées
  yBoule = yBoule + (x_values * vitesseBoule);
  xBoule = xBoule - (y_values * vitesseBoule);
  tft.fillCircle(xBoule, yBoule, rayonBoule, couleurBoule); // on la reaffiche aux nouvelles coordonnées
}

// Fonction de decrementaion du compteur de jeu
bool chronoCalcule() {
  if(chrono < 1) { // Si le chrono est nulle alors :
    start = false; // arreter le jeu
    return false; // retourner faux
  }
  chrono = chrono -10; 
  if(chrono/1000 < (chrono+10)/1000) // sinon si le chrono à changer :
  {
    tft.fillRect(250,10,30,20,couleurBandeau); // on actualise a l'ecran son temps
    tft.drawNumber(chrono/1000, 250, 10); // on actualise a l'ecran son temps
  }
  return true; // on retourne vraie
}

// fonction de copie du tableau
void copie_tableau(unsigned int tab[nbPiecesTab][2])
{
  for(i=0; i<nbPiecesTab; i++) { // pour toutes les pieces du tableau :
    tabActuel[i][0] = tab[i][0]; // on recopie les abscices dans le tableau actuel
    tabActuel[i][1] = tab[i][1]; // on recopie les ordonnées dans le tableau actuel
  }
}

// fonction de copie du tableau
void copie_tableauNoir(unsigned int tab[nbPiecesNoiresTab][2])
{
  for(i=0; i<nbPiecesNoiresTab; i++) { // pour toutes les pieces noires du tableau :
    tabActuelNoir[i][0] = tab[i][0]; // on recopie les abscices dans le tableau actuel des pieces noires
    tabActuelNoir[i][1] = tab[i][1];// on recopie les ordonnées dans le tableau actuel des pieces noires
  }
}

// fonction des victoires
bool victoire(unsigned int tab[nbPiecesTab][2])
{
  for(i=0; i<nbPiecesTab; i++) // pour toutes les pieces du tableau :
    if(tab[i][0]!=1000 || tab[i][1]!=1000) return false; // on verifie si elles ont bien été touché et donc deplacer, si oui on retourne faux
  return true; // sinon on retourne vraie
}

//fonction du menu
void menu() {
  if(digitalRead(WIO_KEY_B) == LOW && mode >=0) { // si le bouton B est appuyé :
    start = true; // on active le jeu
    score = 0; // on initialise les scores a 0
    tft.fillRect(150, 10, 60, 25, couleurBandeau); // on actualise les scores a l'ecran
    tft.drawString(String(score)+"pts", 150, 10); // on actualise les scores a l'ecran
    tft.fillRect(0,40,320,226,fondEcran);  // On enleves les ancienes pieces

    if(mode == 0) { // Si on est sur le mode novice :
      copie_tableau(tabPiecesNov); // Mettre dans le tableau actuel le tableau novice
    }
    if(mode == 1) { // Si on est sur le mode confirme :
      copie_tableau(tabPiecesCon); // Mettre dans le tableau actuel le tableau confirme
    }
    if(mode == 2) { // Si on est sur le mode expert :
      copie_tableau(tabPiecesExp); // Mettre dans le tableau actuel le tableau expert
      copie_tableauNoir(tabPiecesNoires); // Mettre dans le tableau actuel des pieces noires le tableau des pieces noires
      afficherPiecesNoires(tabActuelNoir); // afficher le tableau actuel des pieces noires
    }
    bonus = true; // Initialiser le bonus de temps a vrai
    depart(); // commencer le jeu
  }

  if(digitalRead(WIO_KEY_C) == LOW) { // si le bouton C est appuyé :
    mode = (mode + 1)%3; // calcule le mode de jeu, %3 car il y a trois mode de jeu

    tft.fillRect(0,0,150,40,couleurBandeau);  // Rectangle du bandeau de scores
    tft.fillRect(0,40,320,226,fondEcran);  // Rectangle du fond d'ecran

    if(mode == 0) { // si on est dans le premier mode (novice) alors :
      tft.drawString("novice", 10, 10); // affiche le texte en position (10, 10)
      copie_tableau(tabPiecesNov); // on copie le tableau de pieces novice dans notre tableau actuel
    }
    if(mode == 1) { // si on est dans le deuxieme mode (confirme) alors :
      tft.drawString("confirme", 10, 10); // affiche le texte en position (10, 10)
      copie_tableau(tabPiecesCon); // on copie le tableau de pieces confirme dans notre tableau actuel
    }
    if(mode == 2) { // si on est dans le troisieme mode (expert) alors :
      tft.drawString("expert", 10, 10); // affiche le texte en position (10, 10)
      copie_tableau(tabPiecesExp); // on copie le tableau de pieces expert dans notre tableau actuel
      copie_tableauNoir(tabPiecesNoires); // on copie le tableau de pieces noires expert dans notre tableau actuel noire
      afficherPiecesNoires(tabActuelNoir); // on affiche les pieces noires
    }
    start = false; // on arrete le jeu
    delay(200);
  }

  afficherPiece(tabActuel); // on affiche les pieces
}

void loop() {
  if(start) { // si le jeu commence :
    if(chronoCalcule()) { // si le chrono est actif :
      Gyroscope(); // on active le gyroscope
      
      for(i =0 ; i< nbPiecesTab; i++) // pour toutes les valeurs du tableau actuel de pieces et de pieces noires :
      {
        if(colisions(tabActuel[i][0], tabActuel[i][1])) // s'il y a une collision avec les valeurs du tableau actuel en i alors :
        {
          tft.fillCircle(tabActuel[i][0], tabActuel[i][1],rayonPiece,fondEcran); // Dessin du cercle plein en gris pour cacher la piece
          if(bonus) score = score + 10; // si le bonus n'a pas ete utilise alors les pieces raporte 10 pts
          else score = score + 5; // sinon les pieces rapporte 5 pts
          tone(WIO_BUZZER, 250, 300); // faire un bruit a chaque piece touche
          tft.fillRect(150, 10, 60, 25, couleurBandeau); // actualiser le score a l'ecran
          tft.drawString(String(score)+"pts", 150, 10); // actualiser le score a l'ecran
          tabActuel[i][0] = 1000; // bouger les abscices de la piece hors de l'ecran
          tabActuel[i][1] = 1000; // bouger les ordonnées de la piece hors de l'ecran
        }
        if(mode == 2 && colisions(tabActuelNoir[i][0], tabActuelNoir[i][1])) // s'il y a une collision avec les valeurs du tableau actuel de pieces noires en i alors :
        {
          tft.fillCircle(tabActuelNoir[i][0], tabActuelNoir[i][1],rayonPieceNoire,fondEcran); // Dessin du cercle plein en gris pour cacher la piece
          chrono = chrono - 10000; // perte de 10s sur le chrono
          tone(WIO_BUZZER, 100, 300); // faire un bruit a chaque pieces noires touchées
          tabActuelNoir[i][0] = 1000; // les sortir de l'ecran
          tabActuelNoir[i][1] = 1000; // les sortir de l'ecran
        }
      }

      if(victoire(tabActuel)) // si victoire (plus aucune piece sur l'ecran) alors :
      {
        start = false; // fin de jeu
        if(bonus) score = score + chrono/1000; // ajout du temps au score si le bonus n'a pas ete utilise
        tft.fillRect(150, 10, 60, 25, couleurBandeau); // actualisation du score a l'ecran
        tft.drawString(String(score)+"pts", 150, 10); // actualisation du score a l'ecran
        tone(WIO_BUZZER, 900, 300); // son de victoire
        delay(300); // son de victoire
        tone(WIO_BUZZER, 1100, 300); // son de victoire
        delay(300); // son de victoire
        tone(WIO_BUZZER, 800, 300); // son de victoire
        delay(300); // son de victoire
      }

      if(bonus && digitalRead(WIO_KEY_A) == LOW) { // si la bouton A est appuyé :
        chrono = chrono + 10000; // rajouter 10s au chrono
        bonus = false; // marquer l'utilisation du bonus
      }
    }
    else
    {
      start = false; // sinon arreter le jeu
      tone(WIO_BUZZER, 500, 300); // son de fin de jeu
      delay(300); // delay de 300ms entre chaque son
      tone(WIO_BUZZER, 1700, 400); // son de fin de jeu
      delay(400); // delay de 400ms entre chaque son
      tone(WIO_BUZZER, 1900, 400); // son de fin de jeu
      delay(400); // delay de 400ms entre chaque son
      tone(WIO_BUZZER, 1700, 400); // son de fin de jeu
    }
  }
  else{ // sinon si le jeu ne commence pas alors :
    menu(); // appel a la fonction menu
    chrono = 30000; // definission du chrono a 30s
    xBoule=100; // definission des abscices de la boules rouge
    yBoule=100; // definission des ordonnées de la boules rouge
  }

  delay(10); // delay de 10ms
}