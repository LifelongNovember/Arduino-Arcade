/* ------------------------------- */
/*    Imports des bibliothèques    */
/* ------------------------------- */
#include "Adafruit_ILI9341.h"
#include "Adafruit_GFX.h"
#include "SPI.h"
#include "math.h"

/* ------------------------------- */
/* Ports de l'afficheurs */
/* ------------------------------- */
#define TFT_DC 9
#define TFT_CS 10

/* ------------------------------- */
/* Constante contenant le code héxadécimal des couleurs */
/* Convertion ds couleurs en code RGB565 sur: http://www.barth-dev.de/online/rgb565-color-picker/ */
/* ------------------------------- */
#define BLACK     0x0000
#define BLUE      0x001F
#define RED       0xF800
#define GREEN     0x7700
#define CYAN      0x07FF
#define MAGENTA   0xF81F
#define YELLOW    0xFFE0
#define WHITE     0xFFFF
#define BROWN     0x8200
#define GRAY      0x31A6
#define LIGHTGRAY 0xB596
#define LIME      0x5FA0
#define LIGHTRED  0xFA49

#define SCORING1  0xFCC0
#define SCORING2  0xFE60
#define HIGHLIGHT 0xE71C

/* ------------------------------- */
/* Different type d'orientation de l'écran */
/* ------------------------------- */
#define PORTRAIT 0
#define LANDSCAPE 1

/* ------------------------------- */
/* Calcule du nombre de ligne d'un tableau */
/* ------------------------------- */
#define ROW_COUNT(array)    (sizeof(array) / sizeof(*array))
/* Calcule du nombre de colonne d'un tableau */
#define COLUMN_COUNT(array) (sizeof(array) / (sizeof(**array) * ROW_COUNT(array)))

/* ------------------------------- */
/* Taille d'une case en pixel sur l'écran */
/* ------------------------------- */
#define BOX 20

/* ------------------------------- */
/* Longueur en pixel de l'écran */
#define SCREENWIDTH 320 // Equivalent à tft.width()
/* Largeur en pixel de l'écran */
#define SCREENHEIGHT 240 // Equivalent à tft.height()

/* ------------------------------- */
/* Position en abscice (x) de départ du Pac-Man */
#define PACMANX 2
/* Position en ordonnée (y) de départ du Pac-Man */
#define PACMANY 1

/* ------------------------------- */
/* Position en abscice (x) de départ du Fantome */
#define GHOSTX 4
/* Position en ordonnée (y) de départ du Fantome */
#define GHOSTY 1

/* ------------------------------- */
/* Première position du Pac-Man symbolisant les vies */
/* ------------------------------- */
#define FIRSTLIFE 2

/* ------------------------------- */
/* Nombre maximal de fantôme sur le terrain */
/* ------------------------------- */
#define MAXGHOST 2

/* ------------------------------- */
/* Taille en x du tableau */
#define TABLENGHTX 12
/* Taille en x du tableau */
#define TABLENGHTY 17

/* ------------------------------- */
/* Temps d'attente en millisecondes entre chaque déplacemens */
/* ------------------------------- */
#define DELAYTIME 100

/* ------------------------------- */
/* Frequence des différentes notes de musique */
/* ------------------------------- */
#define DO   65
#define REB  69
#define RE   74
#define MIB  78
#define MI   83
#define FA   87
#define SOLB 93
#define SOL  98
#define LAB  104
#define LA   110
#define SIB  117
#define SI   123


/* ------------------------------- */
/* Définition du Pac-Man */
/* ------------------------------- */
struct Entity {
  char type; // pacman = P ou ghost = G
  double actuX; // Coordonnée X actuelles
  double actuY; // Coordonnée Y actuelles
  double prevX; // Coordonnées X anciennes
  double prevY; // Coordonnée Y anciennes
  char direc; // Direction HAUT = T, BAS = B, GAUCHE = L, Droite = R
  bool invincibility; // Pour le pacman: true = invinsible. Pour le fantôme: true = fuite
};

/* ------------------------------- */
/* Nombre de vie du Pac-Man */
/* ------------------------------- */
int lifepoint;

/* ------------------------------- */
/* Définition d'une partie */
/* ------------------------------- */
struct Party {
  long partyTimeMin; // Champ des minutes pour notre temps
  long partyTimeSec; // Champ des secondes pour notre temps
  long partyTimeMil; // Champ des millisecondes pour notre temps
  int partyScore; // Score totale recolté
  int partyOldScore; // Score juste avant partyScore
  int partyMaxScore; // Score maximal dans une partie
  
};

/* ------------------------------- */
/* Définition d'un objet Afficheur */
/* ------------------------------- */
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);


/* ------------------------------- */
/* Terrain de jeu */
/* ------------------------------- */
char ground[TABLENGHTX][TABLENGHTY];

/* ------------------------------- */
/* Initialisation du Pac-Man */
/* ------------------------------- */
struct Entity pacman;

/* ------------------------------- */
/* Initialisation du Fantôme */
/* ------------------------------- */
struct Entity ghost;

/* ------------------------------- */
/* Initialisation de la party */
/* ------------------------------- */
struct Party party;


/* ------------------------------- */
/* Coordonnées pour faire les designs graphiques */
/* ------------------------------- */
  
  /* ------------------------------- */
  /* Coordonnées du PacMan pour l'affichage */
  /* ------------------------------- */
  int r = 7 ;
  double aa = PI / 4;
  int x1, x2, y1, y2, xR, yR;
  
  /* ------------------------------- */
  /* Coordonnées des Bricks pour l'affichage */
  /* ------------------------------- */
  int wightBrick1 = 9;
  int wightBrick2 = 5;
  int wightBrick3 = 1;
  int heightBrick = 4;

  /* ------------------------------- */
  /* Coordonnées pour les boules de score */
  /* ------------------------------- */
  int rScoring = 5; // Rayon du cercle principal
  int r2Scoring = 4; // Rayon du cercle secondaire
  int r3Scoring = 2; // Rayon du cercle tertiaire 
  int distCircleScoring = 1; // Distance des pics depassant des cercles

  /* ------------------------------- */
  /* Coordonnées pour les boules d'invincibilité */
  /* ------------------------------- */
  int coteLosange = 5; // Ecartement des coins du losange du centre
  // Position du centre des étoiles par rapport au center de la case
  int centerStarX1 = -5;
  int centerStarX2 = 4;
  int centerStarX3 = 5;
  int centerStarY1 = 2;
  int centerStarY2 = -4;
  int centerStarY3 = 4;

  /* ------------------------------- */
  /* Coordonnées pour le crâne */
  /* ------------------------------- */
  int rSkull1 = 30; // Rayon du cercle principal
  int rSkull2 = 22; // Rayon du second cercle faisant office de bouche
  int ySkull2 = 18; // Décalage en y du second cercle par rapport au premier
  int rEye = 8; // Rayon du du cercle des yeux
  int xEye = 12; // Décalage en x du des yeux par rapport au centre
  int yEye = 10; // Décalage en y du des yeux par rapport au centre
  int yNose = 4; // Décalage en y du haut du nez par rapport au second cercle
  int xNoseCorner = 4; // Décalage en x du coin du nez par rapport au haut du nez
  /* ------------------------------- */
  /* Coordonnées pour le bouton */
  /* ------------------------------- */
  int rButton = 12; // Rayon du bouton principal
  int rButton2 = 9; // Rayon de l'effet sur le bouton
  

/* ------------------------------- */
/* Affiche une brick du terrain */
/* ------------------------------- */
void display_brick(int x, int y, int w, int h, int color) {  
  tft.fillRect(x, y, w, h, BLACK);
  
  tft.fillRect(x, y, wightBrick1, heightBrick, color);
  tft.fillRect(x+10, y, wightBrick1, heightBrick, color);
  tft.fillRect(x+BOX, y, wightBrick3, heightBrick, color);
  tft.fillRect(x, y+5, wightBrick2, heightBrick, color);
  tft.fillRect(x+6, y+5, wightBrick1, heightBrick, color);
  tft.fillRect(x+16, y+5, wightBrick2, heightBrick, color);
  tft.fillRect(x, y+10, wightBrick1, heightBrick, color);
  tft.fillRect(x+10, y+10, wightBrick1, heightBrick, color);
  tft.fillRect(x+10, y+10, wightBrick3, heightBrick, color);
  tft.fillRect(x, y+15, wightBrick2, heightBrick, color);
  tft.fillRect(x+6, y+15, wightBrick1, heightBrick, color);
  tft.fillRect(x+16, y+15, wightBrick2, heightBrick, color);
}

/* ------------------------------- */
/* Affiche le terrain suivant le tableau */
/* ------------------------------- */
void display_ground(Entity pacman, Party &party) {
  // Vrai si on sort par la droite
  bool exceedRight = false;
  // vrai si on sort par le bas
  bool exceedBottom = false;
  // Affiche le terrain sans sortir des limites de l'afficheur
  for (int i = 0; i < ROW_COUNT(ground) && !exceedBottom; i++) {
    exceedRight = false;
    /*if (BOX*(i+1) >= SCREENHEIGHT)
      exceedBottom = true;*/
    for (int j = 0; j < COLUMN_COUNT(ground) && !exceedRight; j++) {
      if (ground[i][j] == '#') 
        display_brick(j*BOX-(BOX/2), i*BOX+(BOX/2), BOX, BOX, BROWN); 
      else if (ground[i][j] == 'o' && !collectablePoint(j, i, pacman)) {
        display_scoringBalls(j*BOX, (i+1)*BOX);
        party.partyMaxScore ++;
      }
      /*if (BOX*(j+1) >= SCREENWIDTH)
        exceedRight = true;*/
    }
  }
}

/* ------------------------------- */
/* Affiche le menu GAMEOVER */
/* ------------------------------- */
void display_badEnd() {
  int backColor = LIGHTGRAY;
  tft.fillScreen(backColor);
  tft.setCursor(SCREENWIDTH/5  ,SCREENHEIGHT/8);
  tft.setTextSize(4);
  tft.setTextColor(BLACK);
  tft.print("GAMEOVER");
  
  dislay_skull(SCREENWIDTH/2, (SCREENHEIGHT*3)/7, backColor);
  
  tft.setTextSize(2);
  tft.setCursor(SCREENWIDTH/9, (SCREENHEIGHT*3)/5);
  tft.print("REJOUER ?");

  tft.setCursor((SCREENWIDTH*5)/9, (SCREENHEIGHT*3)/5);
  tft.print("QUITTER ?");

  tft.setCursor((SCREENWIDTH*2)/5, (SCREENHEIGHT*3)/4);
  tft.print("PRESS");

  display_button((SCREENWIDTH*2)/7  , (SCREENHEIGHT*3)/4, GREEN, LIME);
  display_button((SCREENWIDTH*2)/3, (SCREENHEIGHT*3)/4, RED, LIGHTRED);
}

/* ------------------------------- */
/* Affiche un crâne */
/* ------------------------------- */
void dislay_skull(int x, int y, int color) {
  tft.fillCircle(x, y, rSkull1, BLACK);
  tft.fillCircle(x, y+ySkull2, rSkull2, BLACK);
  
  tft.fillCircle(x+ xEye, y+yEye, rEye, WHITE);
  tft.fillCircle(x-xEye, y+yEye, rEye, WHITE);

  tft.fillRect(x-xNoseCorner, y+ySkull2, xNoseCorner*2+1, yNose*2, WHITE);
  tft.fillTriangle(x, y+ySkull2-yNose, x-xNoseCorner, y+ySkull2, x+xNoseCorner, y+ySkull2, WHITE);
}

/* ------------------------------- */
/* Affiche un bouton */
/* ------------------------------- */
void display_button(int x, int y, int color, int color2) {
  tft.fillCircle(x, y, rButton, color);
  tft.fillCircle(x, y, rButton2, WHITE);
  tft.fillCircle(x-1, y-1, rButton2, LIGHTGRAY);
  tft.fillCircle(x-1, y-1, rButton2-1, color2);
}

/* ------------------------------- */
/* Affiche le menu de victoire */
/* ------------------------------- */
void display_goodEnd() {
  tft.fillScreen(LIGHTGRAY);
  tft.setCursor(SCREENWIDTH/5 ,SCREENHEIGHT/3);
  tft.setTextSize(4);
  tft.setTextColor(BLACK);
  tft.print("VICTORY");
}

/* ------------------------------- */
/* Affiche le menu en haut de l'écran */
/* ------------------------------- */
void display_topMenu() {
  tft.fillRect(0, 0, SCREENWIDTH, BOX, LIGHTGRAY);
}

/* ------------------------------- */
/* Affiche les différentes informations du menu en haut de l'écran */
/* ------------------------------- */
void display_topMenuLabel() {
  int y = BOX/3;
  int x = 5;
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  
  for (int i = 0; i < lifepoint; i++) {
    tft.setCursor(x, y);
    if (i == 0) {
      tft.print("LIFE ");
      x = x + 2*BOX;
    } else if (i == 1) {
      tft.print("SCORE ");
      x = x + BOX;
    } else if (i == 2) {
      tft.print("TIME ");
    }
    x = x + lifepoint*BOX;
  }
}

/* ------------------------------- */
/* Affiche le nombre de vies restantes */
/* ------------------------------- */
void display_topMenuLife(Entity pacman) {
  pacman.actuY= -0.5;
  for (int i = 0; i < lifepoint; i++) {
    pacman.actuX = FIRSTLIFE + i;
    display_pacman(pacman, LIGHTGRAY);
  }
}

/* ------------------------------- */
/* Affiche le nombre de vies restantes */
/* ------------------------------- */
void display_deleteLife() {
  tft.fillRect((lifepoint + 1)*BOX+(BOX/2), 0, BOX, BOX, LIGHTGRAY);
}

/* ------------------------------- */
/* Affiche le score de la party */
/* ------------------------------- */
void display_topMenuScore(Party &party) {
  tft.setCursor(SCREENWIDTH/2-BOX, BOX/10);
  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  
  if (party.partyScore == 0 && party.partyScore != party.partyOldScore) {
    tft.fillRect(SCREENWIDTH/2-BOX, 0, BOX*2, BOX, LIGHTGRAY);
    tft.print(party.partyScore);
    party.partyOldScore = party.partyScore;
  } else if (party.partyScore != party.partyOldScore) {
    tft.fillRect(SCREENWIDTH/2-BOX, 0, BOX*2, BOX, LIGHTGRAY);
    tft.print(party.partyScore);
    party.partyOldScore = party.partyScore;
  }
}

/* ------------------------------- */
/* Affiche le temps de la party */
/* ------------------------------- */
void display_topMenuTime(Party &party) {
  tft.fillRect(SCREENWIDTH-BOX*5, 0, BOX*7, BOX, LIGHTGRAY);
  tft.setCursor(SCREENWIDTH-BOX*5, BOX/10);
  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.print(party.partyTimeMin);
  tft.print(":");
  tft.print(party.partyTimeSec);
  tft.print(":");
  tft.print(party.partyTimeMil);
  timeCalculation(party);
  delay(100);
}

/* ------------------------------- */
/* Affiche le Pac-Man */
/* ------------------------------- */
void display_pacman(Entity &pacman, int backgroundColor) {
  int x = (pacman.actuX*BOX);
  int y = (pacman.actuY*BOX)+BOX;
  if (pacman.direc == 'R') {
    x1 = x + r;
    x2 = x + r;
    y1 = y - r;
    y2 = y + r;
  } else if (pacman.direc == 'L') {
    x1 = x - r;
    x2 = x - r;
    y1 = y - r;
    y2 = y + r;
  } else if (pacman.direc == 'T') {
    x1 = x + r;
    x2 = x - r;
    y1 = y - r;
    y2 = y - r;
  } else if (pacman.direc == 'B') {
    x1 = x - r;
    x2 = x + r;
    y1 = y + r;
    y2 = y + r;
  }
  tft.fillCircle(x, y, r, YELLOW);
  tft.fillTriangle(x, y, x1, y1, x2, y2, backgroundColor);
}

/* ------------------------------- */
/* Affiche un Fantome */
/* ------------------------------- */
void display_ghost(int x, int y) {
  tft.fillCircle(x, y, 7, RED);
}

/* ------------------------------- */
/* Affiche un carré pour caché le Pac-Man */
/* ------------------------------- */
void display_brick(int x, int y, int w, int l) {
  tft.fillRect(x, y, w, l, GRAY);
}

/* ------------------------------- */
/* Affiche une boule de score */
/* ------------------------------- */
void display_scoringBalls(int x, int y) {
  tft.fillCircle(x, y, rScoring, SCORING1);
  tft.fillCircle(x, y, r2Scoring, SCORING2);
  tft.fillCircle(x, y, r3Scoring, WHITE);

  tft.drawLine(x-r2Scoring, y-distCircleScoring, x-distCircleScoring, y-r2Scoring, HIGHLIGHT);
  tft.drawLine(x-r2Scoring, y-distCircleScoring*2, x-distCircleScoring*2, y-r2Scoring, HIGHLIGHT);
}

/* ------------------------------- */
/* Affiche un boule d'invinsibilité */
/* ------------------------------- */
void display_invinsibilityBalls(int x, int y) {
  /*int color;
  // Losange de plus en plus petit
  for (int i = coteLosange; i > 1; i--) {       // PEUT ËTRE UN BUG AVEC COTELOSANGE ET COTELOSANGE-1
    // Losange blanc du fond
    if (i == coteLosange) 
      color = WHITE;
    // Losange orange par dessus
    else if (i == coteLosange-1)
      color = ORANGE;
    // Losange jaune par dessus
    else if (i == colorLosange-2)
      color = YELLOW;
    // Losange blanc par dessus
    else
      color = WHITE;
    tft.fillTriangle(x-i, y, x, y-i, x, y+i, color);
    tft.fillTriangle(x+i, y, x, y-i, x, y+i, color);
  }
  tft.fillTriangle(x+centerStarX1-1, y-centerStarX1, x+centerStarX1, y-centerStarY1-1, x+centerStarX1, y-centerStar+1, color);
  tft.fillTriangle(x+centerStarX1-1, y-centerStarX1, x+centerStarX1, y-centerStarY1-1, x+centerStarX1, y-centerStar+1, color);*/
}


/* ------------------------------- */
/* Caractéristiques d'un son */
/* ------------------------------- */

  /* ------------------------------- */
  /* Fréquence du son */
  /* ------------------------------- */
  int frequence;

  /* ------------------------------- */
  /* Tempo */
  /* ------------------------------- */
  int tempo;

  /* ------------------------------- */
  /* Duree de la note */
  /* ------------------------------- */
  int duree = 0;


/* ------------------------------- */
/* Différents sons du jeu */
/* ------------------------------- */

  /* ------------------------------- */
  /* Son à la mort du Pac-Man */
  /* ------------------------------- */
  const int lengthStartPacman = 41;
  const int tempoStartPacman = 200;
  // Première colonne: Ton 
  // Deuxième colonne: Octave
  // Troisième colonne: Rythme
  int startPacman[lengthStartPacman][3] = {
    DO,   3, 50,
    DO,   4, 50,
    SOL,  3, 50,
    MI,   3, 50,
    DO,   4, 25,
    SOL,  3, 25,
    MI,   3, 50,
    MI,   2, 60,
    0,    0, 40,
    REB,  3, 50,
    REB,  4, 50,
    LAB,  3, 50,
    FA,   3, 50,
    RE,   4, 25,
    LA,   3, 25,
    FA,   3, 50,
    FA,   2, 60,
    0,    0, 40,
    DO,   3, 50,
    DO,   4, 50,
    SOL,  3, 50,
    MI,   3, 50,
    DO,   4, 25,
    SOL,  3, 25,
    MI,   3, 50,
    MI,   2, 60,
    0,    0, 40,
    MI,   3, 25,
    FA,   3, 25,
    SOLB, 3, 25,
    0,    0, 20,
    FA,   3, 25,
    SOL,  3, 25,
    LAB,  3, 25,
    0,    0, 20,
    SOL,  3, 25,
    LA,   3, 25,
    SIB,  3, 25,
    0,    0, 20,
    DO,   4, 75,
    0,    0, 0
  };

  /* ------------------------------- */
  /* Son au ramassage de score */
  /* ------------------------------- */
  const int lengthKeepScore = 5;
  const int tempoKeepScore = 200;
  // Première colonne: Ton 
  // Deuxième colonne: Octave
  // Troisième colonne: Rythme
  int keepScore[lengthKeepScore][3] = {
    DO,   4, 10,
    SOL,  4, 10,
    DO,   4, 10, 
    SOL,  4, 10,
    0,    0, 0
  };


/* ------------------------------- */
/* Retourne vrai si il y a une collision avec le terrain */
/* ------------------------------- */
bool isAlive(Entity pacman) {
  if (lifepoint > 0) {
    return true;
  } else {
    return false;
  }
}

/* ------------------------------- */
/* Retourne vrai si il y a une collision avec le terrain */
/* ------------------------------- */
bool occuped(int x, int y) {
  if (ground[y][x] == '#') {
    return true;
  } else {
    return false;
  }
}

/* ------------------------------- */
/* Retourne vrai si il y a une collision avec un collectable et remplace la case du tableau correspondante */
/* ------------------------------- */
bool collectablePoint(int x, int y, Entity entity) {
  if (entity.type == 'G') {
    if (ground[y][x] == 'o' && (x == entity.prevX && y == entity.prevY))
      return true;
    else
      return false;
  }
  if (ground[y][x] == 'o' && (x == entity.actuX && y == entity.actuY)) {
    ground[y][x] = 'x';
    return true;
  } else {
    return false;
  }
}

/* ------------------------------- */
/* Retourne vrai si il y a une collision entre deux Entité */
/* ------------------------------- */
bool entityCollision(Entity pacman, Entity ghost) {
  if ((pacman.prevX == ghost.prevX && pacman.prevY == ghost.prevY) || 
      (pacman.actuX == ghost.prevX && pacman.actuY == ghost.prevY)|| 
      (pacman.prevX == ghost.actuX && pacman.prevY == ghost.actuY)) {
    return true;
  } else {
    return false;
  }
}

/* ------------------------------- */
/* Affiche le Pac-Man à la bonne position suivant les collisions */
/* ------------------------------- */
// Affiche le PacMan et les fantôme à la bonne position s'il n'y a pas de collision
void applyOccuped(Entity &pacman, Entity &ghost, Party &party) {

  // Si il y a une collision entre un Pac-Man et un Fantome et qu'il lui reste de la vie
  if (entityCollision(pacman, ghost) && isAlive(pacman)) {
    // place le Pac-Man à sa position de départ et lui retire une vie
    if (!pacman.invincibility && !ghost.invincibility) {
      pacman.actuX = PACMANX;
      pacman.actuY = PACMANY;
      lifepoint--;
      display_deleteLife();
    // Place le Fantôme à sa position de départ
    } else {
      ghost.actuX = GHOSTX;
      ghost.actuY = GHOSTY;
    }
  }
  
  if (entityCollision(pacman, ghost) && !pacman.invincibility && !ghost.invincibility && !isAlive(pacman)) {
    pacman.actuX = PACMANX;
    pacman.actuY = PACMANY;
    lifepoint--;
    display_deleteLife();
  } 

  // Si il y a une collision entre un Pac-Man et une boule de score, incrémente le score
  if (collectablePoint(pacman.actuX, pacman.actuY, pacman)) {
    party.partyScore++;
    sounds(keepScore, tempoKeepScore, lengthKeepScore);
  }
  
  // Si il y a une collision avec le terrain, le Pac-Man retoure à la position précédente
  if (occuped(pacman.actuX, pacman.actuY)) { 
    pacman.actuY = pacman.prevY;
    pacman.actuX = pacman.prevX;
  } else { 
    if ((pacman.actuX != pacman.prevX || pacman.actuY != pacman.prevY)) {
      // Suppression de l'affichage du pacman à son ancienne position
      display_brick(pacman.prevX*BOX-(BOX/2), pacman.prevY*BOX+(BOX/2), BOX, BOX);
      // Ajout de l'affichage du pacman à sa nouvelle position
      display_pacman(pacman, GRAY);
    } 
  }
  
  // Si il y a une collision avec le terrain, le Fantome retoure à la position précédente
  if (occuped(ghost.actuX, ghost.actuY)) { 
    ghost.actuY = ghost.prevY;
    ghost.actuX = ghost.prevX;
  } else { 
    if ((ghost.actuX != ghost.prevX || ghost.actuY != ghost.prevY)) {
      // Suppression de l'affichage du Fantome à son ancienne position
      display_brick(ghost.prevX*BOX-(BOX/2), ghost.prevY*BOX+(BOX/2), BOX, BOX);
      // Ajout de l'affichage de l'ancienne boule de score
      if (collectablePoint(ghost.prevX, ghost.prevY, ghost)) {
        display_scoringBalls((ghost.prevX)*BOX, (ghost.prevY+1)*BOX);
      }
      // Ajout de l'affichage du Fantome à sa nouvelle position
      display_ghost(ghost.actuX*BOX, ghost.actuY*BOX+BOX);
    } 
  }
}

/* ------------------------------- */
/* Modifie la position et l'orientation du Pac-Man suivant les directions */
/* ------------------------------- */
// Modifie la position du PacMan suivant la position du joystick
void testInput(Entity &pacman, Entity &ghost) {
  // Modifie la position et la direction du pacman suivant l'orientation du joystick
  pacman.prevY = pacman.actuY;
  pacman.prevX = pacman.actuX;
  if (digitalRead(31) || (digitalRead(31) && digitalRead(28)) || (digitalRead(31) && digitalRead(29))) {
    pacman.direc = 'L';
    pacman.actuX--;
  } else if (digitalRead(30) || (digitalRead(30) && digitalRead(28)) || (digitalRead(30) && digitalRead(29))) {
    pacman.direc = 'R';
    pacman.actuX++;
  } else if (digitalRead(29)) {
    pacman.direc = 'T';
    pacman.actuY--;
  } else if (digitalRead(28)) {
    pacman.direc = 'B';
    pacman.actuY++;
  }

  // Modifie la position et la direction du ghost suivant les bouton appuyés
  ghost.prevY = ghost.actuY;
  ghost.prevX = ghost.actuX;
  if (digitalRead(25)) {
    ghost.direc = 'L';
    ghost.actuX--;
  } else if (digitalRead(22)) {
    ghost.direc = 'R';
    ghost.actuX++;
  } else if (digitalRead(23)) {
    ghost.direc = 'T';
    ghost.actuY--;
  } else if (digitalRead(24)) {
    ghost.direc = 'B';
    ghost.actuY++;
  }
}

/* ------------------------------- */
/* Modifie le temps de la partie toute les 100 millisecondes */
/* ------------------------------- */
void timeCalculation(Party &party) {
  if (party.partyTimeMil < 9) {
    party.partyTimeMil++;
  } else {
    party.partyTimeMil = 0;
    if (party.partyTimeSec < 59) {
      party.partyTimeSec++;
    } else {
      party.partyTimeSec = 0;
      party.partyTimeMin++;
    }
  }
}

/* ------------------------------- */
/* Produit un son à la mort du Pac-Man */
/* ------------------------------- */
void sounds(int tab[][3], int tempo, int lengthTab) {
  unsigned long tempsDep = millis();
  unsigned long tempsAct;
  int i = 0;
  while (i < lengthTab) {
    tempsAct = millis();
    if (tempsAct - tempsDep >= duree) {
      noTone(32); // Fin du son
      delay(10); // Délai pour l'attaque 

      frequence = tab[i][0] * pow(2, tab[i][1]);
      
      duree = 1000 / (tempo / 60) * (float(tab[i][2]) / 100);
      if (frequence > 0)
        tone(32, frequence);
      i++;
      tempsDep = tempsAct;
    }
  }
}


/* ------------------------------- */
/* Initialise une partie de Pac-Man */
/* ------------------------------- */
void initGame() {
  // Point de vie du Pac-Man
  lifepoint = 3;

  // Terrain de jeu
  char groundTmp[TABLENGHTX][TABLENGHTY] = {
    {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'},
    {'#', 'o', 'o', 'o', 'o', '#', 'o', 'o', 'o', 'o', 'o', '#', 'o', 'o', '#', 'o', '#'},
    {'#', 'o', '#', '#', 'o', '#', 'o', '#', '#', '#', 'o', 'o', 'o', 'o', 'o', 'o', '#'},
    {'#', 'o', 'o', 'o', 'o', '#', 'o', 'o', 'o', '#', '#', '#', '#', 'o', '#', 'o', '#'},
    {'#', '#', 'o', '#', 'o', 'o', 'o', '#', 'o', '#', 'o', 'o', 'o', 'o', 'o', '#', '#'},
    {'#', 'o', 'o', 'o', 'o', '#', '#', '#', 'o', '#', 'o', '#', 'o', '#', 'o', 'o', '#'},
    {'#', 'o', '#', '#', 'o', 'o', 'o', 'o', 'o', '#', 'o', '#', 'o', '#', '#', 'o', '#'},
    {'#', 'o', 'o', 'o', 'o', '#', '#', '#', 'o', '#', 'o', '#', 'o', '#', 'o', 'o', '#'},
    {'#', '#', 'o', '#', 'o', 'o', 'o', '#', 'o', '#', 'o', 'o', 'o', 'o', 'o', '#', '#'},
    {'#', 'o', 'o', 'o', 'o', '#', 'o', 'o', 'o', '#', '#', '#', '#', 'o', '#', '#', '#'},
    {'#', 'o', 'o', 'o', 'o', '#', 'o', 'o', 'o', 'o', 'o', '#', 'o', 'o', '#', '#', '#'},
    {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'}, 
  };
  memcpy(ground, groundTmp, sizeof(char)*TABLENGHTX*TABLENGHTY);

  // Initialisation du Pac-Man
  pacman = {
    'P',
    PACMANX, PACMANY,
    // Position hors du terrain car il n'a pas encore bougé 
    -PACMANX, -PACMANY, 
    'R',
    false
  };

  // Initialisation du Fantôme
  ghost = {
    'G',
    GHOSTX, GHOSTY,
    // Position hors du terrain car il n'a pas encore bougé 
    -GHOSTX, -GHOSTY, 
    'L',
    false
  };

  // Initialisation de la partie
  party = {
    0, // Nombre de minutes
    0, // Nombre de secondes
    0, // Nombre de millisecondes
    0, // Score actuel
    -1, // Score avant le score actuel
    0 // Score maximal de la partie, incrémenté à chaque ajout de boule de score
  };

  // Appel des fonctions pour l'affichage
  tft.fillScreen(GRAY);
  display_ground(pacman, party);
  display_topMenu();
  display_topMenuLabel();
  display_topMenuLife(pacman);
  //display_invinsibilityBalls();
}

/* ------------------------------- */
/* Setup */
/* ------------------------------- */
void setup() {
  Serial.begin(9600);
  tft.begin();
  tft.setRotation(LANDSCAPE);
  
  pinMode(28, INPUT); pinMode(24, INPUT); // Bas
  pinMode(29, INPUT); pinMode(23, INPUT); // Haut
  pinMode(30, INPUT); pinMode(22, INPUT); // Droite
  pinMode(31, INPUT); pinMode(25, INPUT); // Gauche
  pinMode(26, INPUT); // Bouton START (vert)
  pinMode(27, INPUT ); // Bouton SELECT (rouge)
  pinMode(32, OUTPUT); // Haut-parleur
  sounds(startPacman, tempoStartPacman, lengthStartPacman);
  initGame();
}

/* ------------------------------- */
/* Loop */
/* ------------------------------- */
void loop() {
  // Tant que le Pac-Man a des vies
  while (isAlive(pacman) && party.partyScore != party.partyMaxScore) {
    display_topMenuTime(party);
    display_topMenuScore(party);
    applyOccuped(pacman, ghost, party);
    testInput(pacman, ghost);
  }
  if (!isAlive(pacman))
    // Fin de la partie car plus de vie
    display_badEnd();
  else if (party.partyScore == party.partyMaxScore)
    // Fin de la partie car score au max
    tft.fillRect(0, 0, SCREENWIDTH, SCREENHEIGHT, RED);
  while (!digitalRead(27)) {
  };
  sounds(startPacman, tempoStartPacman, lengthStartPacman);
  initGame();
}

// tft.dear(COLOR) efface l'écran avec la couleur voulue
// tft.RGB(r, g, b) retourne une couleur au format RGB


/*
Petit clin d'oeil mr Lukas
Nanomaître 2015
*//*
// constantes de fréquences
#define DON 33
#define REB 35
#define REN 37
#define MIB 39
#define MIN 41
#define FAN 44
#define SOB 46
#define SON 49
#define LAB 52
#define LAN 55
#define SIB 58
#define SIN 62

// tableau pour la mélodie
int part[50][3] = {
  DON, 2, 100,
  DON, 2, 100,
  DON, 2, 100,
  LAB, 1, 75,
  MIB, 2, 25,
  DON, 2, 100,
  LAB, 1, 75,
  MIB, 2, 25,
  DON, 2, 200,
  SON, 2, 100,
  SON, 2, 100,
  SON, 2, 100,
  LAB, 2, 75,
  MIB, 2, 25,
  SIN, 1, 100,
  LAB, 1, 75,
  MIB, 2, 25,
  DON, 2, 200,
  DON, 3, 100,
  DON, 2, 100,
  DON, 3, 25,
  SIN, 2, 25,
  DON, 3, 25,
  0, 0, 75,
  SIN, 2, 50,
  SIB, 2, 100,
  SIB, 1, 100,
  SON, 2, 25,
  SOB, 2, 25,
  SON, 2, 25,
  0, 0, 75,
  SOB, 2, 50,
  FAN, 2, 100,
  SON, 1, 100,
  SIB, 1, 100,
  SON, 1, 75,
  MIB, 2, 25,
  DON, 2, 100,
  LAB, 1, 75,
  MIB, 2, 25,
  DON, 2, 200,
  -1
};
int pinSon = 3; // pin de connection du haut-parleur
int tempo = 120; // variable du tempo
int duree = 0; // variable de durée de note
unsigned long tempsDep; // variable de temps de départ
int p = 0; // variable de position dans le tableau de mélodie
void setup() {
  pinMode(pinSon,OUTPUT); 
  tempsDep = millis(); // initialisation du temps de départ
}

void loop() {
  joue(); // appel de la fonction pour jouer la mélodie
}

//fonction de lecture de la mélodie
void joue() {
  unsigned long tempsAct = millis();
  if (tempsAct - tempsDep >= duree) {
    if (part[p][0] != -1) { // test de fin de tableau
      noTone(pinSon);
      delay(10); // délai pour l'attaque
      // la fréquence est calculée en fonction des fréquences de base
      // et de l'octave définit dans le tableau
      int frequence = part[p][0] * pow(2, part[p][1] + 1);
      // la durée de la note est calculée comme en musique
      duree = 1000 / (tempo / 60) * (float(part[p][2]) / 100);
      if (frequence > 0) {
        tone (pinSon, frequence);
      }
      p++; //incrémentation de la position dans le tableau
    }
    else { 
      noTone(pinSon);
      p=0;// retour au début du tableau
      duree=1000;// attente avant répétition
    }
    tempsDep=tempsAct;
  }
}*/
