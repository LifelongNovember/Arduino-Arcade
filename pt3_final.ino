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
#define BLACK         0x0000
#define BLUE          0x001F
#define RED           0xF800
#define GREEN         0x7700
#define CYAN          0x07FF
#define MAGENTA       0xF81F
#define YELLOW        0xFFE0
#define WHITE         0xFFFF
#define BROWN         0x8200
#define GRAY          0x31A6
#define LIGHTGRAY     0xB596
#define VERYLIGHTGRAY 0xAD75
#define LIME          0x5FA0
#define LIGHTRED      0xFA49
#define ORANGE        0xFC08

#define SCORING1      0xFCC0
#define SCORING2      0xFE60
#define HIGHLIGHT     0xE71C

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
#define GHOSTX 6
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
/* Nombre maximal de boules d'invinsibilité sur le terrain */
/* ------------------------------- */
#define MAXINVINC 2

/* ------------------------------- */
/* Nombre maximal de boules d'invinsibilité sur le terrain en centaine de millisecondes */
/* ------------------------------- */
#define TIMEINVINC 60

/* ------------------------------- */
/* Taille en x du terrain */
#define TABLENGHTX 12
/* Taille en y du terrain */
#define TABLENGHTY 17

/* ------------------------------- */
/* Nombre de jeu du menu d'acceuil */
/* ------------------------------- */
#define GAMESLENGHT (SCREENWIDTH/ICONE) * (SCREENHEIGHT/ICONE) // 320/80 * 240/80 
// Avec des cases de 80x80 pixels

/* ------------------------------- */
/* Taille d'une icône */
/* ------------------------------- */
#define ICONE 80

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
/* Contient la liste des jeux */
/* ------------------------------- */
char games[GAMESLENGHT];


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
  int invincibility; // Pour le pacman: supérieur à 0 = invinsible. Pour le fantôme: supérieur à 0 = fuite
};

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
  int partyLifepoint; // Nombre de vie restant au Pac-Man
};

/* ------------------------------- */
/* Nombre de boules d'invinsibilité sur le terrain */
/* ------------------------------- */
int nbInvinc;

/* ------------------------------- */
/* Définition d'un objet Afficheur */
/* ------------------------------- */
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);


/* ------------------------------- */
/* Terrain de jeu */
/* ------------------------------- */
char* game[GAMESLENGHT] = {
  "pacman", "", "", "", "", "", "", "", "", "", "", ""
};

/* ------------------------------- */
/* Coordonnées du jeu selectionné */
/* ------------------------------- */
int selectedGameX = 0;
int selectedGameY = 0;

/* ------------------------------- */
/* Ancienne coordonnées du jeu selectionné */
/* ------------------------------- */
int oldSelectedGameX = 0;
int oldSelectedGameY = 0;

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
  /* Coordonnées pour l'icône Pac-Man */
  /* ------------------------------- */
    int rIconePacman = 20;
  
  /* ------------------------------- */
  /* Coordonnées du PacMan pour l'affichage */
  /* ------------------------------- */
  int rPacman = 7;
  int xPacman1, xPacman2, yPacman1, yPacman2;

  /* ------------------------------- */
  /* Coordonnées du fantôme pour l'affichage */
  /* ------------------------------- */
  int rGhost = 7;
  int rGhostEyes = 3;
  int xGhostEye1 = -1;
  int xGhostEye2 = 3;
  int yGhostEye = -1;
  
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
  int rButton = 24; // Rayon du bouton principal
  int rButton2 = 18; // Rayon de l'effet sur le bouton

  /* ------------------------------- */
  /* Coordonnées pour l'étoile */
  /* ------------------------------- */
  int decaSquare = 4*4; // Décalage par rapport au centre du carré
  // Décalage des différente coordonnées du triangle autour du carré
  int decaTriangle1_2_3 = 11*4;
  int xDecaTriangle4_5 = 7*4;
  int yDecaTriangle4_5 = 9*4;
  

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
  // Affiche le terrain sans sortir des limites de l'afficheur
  for (int i = 0; i < ROW_COUNT(ground); i++) {
    for (int j = 0; j < COLUMN_COUNT(ground); j++) {
      if (ground[i][j] == '#') 
        display_brick(j*BOX-(BOX/2), i*BOX+(BOX/2), BOX, BOX, BROWN); 
      else if (ground[i][j] == 'o' && !collectablePoint(j, i, pacman)) {
        display_scoringBalls(j*BOX, (i+1)*BOX);
        party.partyMaxScore++;
      }
    }
  }

  for (int i = 0; i < ROW_COUNT(ground); i++) {
    for (int j = 0; j < COLUMN_COUNT(ground); j++) {
      if (ground[i][j] == 'o' && random(0, party.partyMaxScore) < 2 && nbInvinc < MAXINVINC) {
        display_clear(j*BOX-(BOX/2), i*BOX+(BOX/2), BOX, BOX);
        ground[i][j] = '&';
        display_invinsibilityBalls(j*BOX, (i+1)*BOX);
        party.partyMaxScore--;
        nbInvinc++;
      }
    }
  }
}

/* ------------------------------- */
/* Affiche le menu GAMEOVER */
/* ------------------------------- */
void display_badEnd() {
  tft.fillScreen(LIGHTGRAY);
  tft.setCursor(SCREENWIDTH/5, SCREENHEIGHT/8);
  tft.setTextSize(4);
  tft.setTextColor(BLACK);
  tft.print("GAMEOVER");
  
  dislay_skull(SCREENWIDTH/2, (SCREENHEIGHT*3)/7);
  
  tft.setTextSize(2);
  tft.setCursor(SCREENWIDTH/9, (SCREENHEIGHT*3)/5);
  tft.print("REPLAY ?");

  tft.setCursor((SCREENWIDTH*4)/7, (SCREENHEIGHT*3)/5);
  tft.print("QUIT ?");

  tft.setCursor((SCREENWIDTH*2)/5, (SCREENHEIGHT*3)/4);
  tft.print("PRESS");

  display_button(SCREENWIDTH/5, (SCREENHEIGHT*4)/5, GREEN, LIME);
  display_button((SCREENWIDTH*4)/5, (SCREENHEIGHT*4)/5, RED, LIGHTRED);
}

/* ------------------------------- */
/* Affiche un crâne */
/* ------------------------------- */
void dislay_skull(int x, int y) {
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
  int backColor = LIGHTGRAY;
  tft.fillScreen(backColor);
  tft.setCursor(SCREENWIDTH/4, SCREENHEIGHT/8);
  tft.setTextSize(4);
  tft.setTextColor(BLACK);
  display_stars(SCREENWIDTH/2, SCREENHEIGHT/2, YELLOW);
  tft.print("VICTORY");
  
  tft.setTextSize(2);
  tft.setCursor(SCREENWIDTH/9, (SCREENHEIGHT*3)/5);
  tft.print("REPLAY ?");

  tft.setCursor((SCREENWIDTH*4)/7, (SCREENHEIGHT*3)/5);
  tft.print("QUIT ?");

  tft.setCursor((SCREENWIDTH*2)/5, (SCREENHEIGHT*3)/4);
  tft.print("PRESS");

  display_button(SCREENWIDTH/5  , (SCREENHEIGHT*4)/5, GREEN, LIME);
  display_button((SCREENWIDTH*4)/5, (SCREENHEIGHT*4)/5, RED, LIGHTRED);
}

/* ------------------------------- */
/* Affiche une étoile */
/* ------------------------------- */
void display_stars(int x, int y, int color) {
  tft.fillRect(x-decaSquare, y-decaSquare, decaSquare*2+1, decaSquare*2+1, color);
  tft.fillTriangle(x, y-decaTriangle1_2_3-decaSquare, x-decaSquare, y-decaSquare, x+decaSquare, y-decaSquare, color);
  tft.fillTriangle(x+decaSquare, y-decaSquare, x+decaSquare+decaTriangle1_2_3, y-decaSquare, x+decaSquare, y+decaSquare, color);
  tft.fillTriangle(x-decaSquare, y-decaSquare, x-decaSquare-decaTriangle1_2_3, y-decaSquare, x-decaSquare, y+decaSquare, color);
  tft.fillTriangle(x+decaSquare, y-decaSquare, x+decaSquare+xDecaTriangle4_5, y+decaSquare+yDecaTriangle4_5, x-decaSquare, y+decaSquare, color);
  tft.fillTriangle(x-decaSquare, y-decaSquare, x-decaSquare-xDecaTriangle4_5, y+decaSquare+yDecaTriangle4_5, x+decaSquare, y+decaSquare, color);

  tft.fillRect(x-decaSquare, y-decaSquare, decaSquare*2+1, decaSquare*2+1, color);
  tft.fillTriangle(x, y-decaTriangle1_2_3-decaSquare, x-decaSquare, y-decaSquare, x+decaSquare, y-decaSquare, color);
  tft.fillTriangle(x+decaSquare, y-decaSquare, x+decaSquare+decaTriangle1_2_3, y-decaSquare, x+decaSquare, y+decaSquare, color);
  tft.fillTriangle(x-decaSquare, y-decaSquare, x-decaSquare-decaTriangle1_2_3, y-decaSquare, x-decaSquare, y+decaSquare, color);
  tft.fillTriangle(x+decaSquare, y-decaSquare, x+decaSquare+xDecaTriangle4_5, y+decaSquare+yDecaTriangle4_5, x-decaSquare, y+decaSquare, color);
  tft.fillTriangle(x-decaSquare, y-decaSquare, x-decaSquare-xDecaTriangle4_5, y+decaSquare+yDecaTriangle4_5, x+decaSquare, y+decaSquare, color);
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
  
  for (int i = 0; i < party.partyLifepoint; i++) {
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
    x = x + party.partyLifepoint*BOX;
  }
}

/* ------------------------------- */
/* Affiche le nombre de vies restantes */
/* ------------------------------- */
void display_topMenuLife(Entity pacman) {
  pacman.actuY= -0.5;
  for (int i = 0; i < party.partyLifepoint; i++) {
    pacman.actuX = FIRSTLIFE + i;
    display_pacman(pacman, LIGHTGRAY);
  }
}

/* ------------------------------- */
/* Affiche le nombre de vies restantes */
/* ------------------------------- */
void display_deleteLife() {
  tft.fillRect((party.partyLifepoint + 1)*BOX+(BOX/2), 0, BOX, BOX, LIGHTGRAY);
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
    xPacman1 = x + rPacman;
    xPacman2 = x + rPacman;
    yPacman1 = y - rPacman;
    yPacman2 = y + rPacman;
  } else if (pacman.direc == 'L') {
    xPacman1 = x - rPacman;
    xPacman2 = x - rPacman;
    yPacman1 = y - rPacman;
    yPacman2 = y + rPacman;
  } else if (pacman.direc == 'T') {
    xPacman1 = x + rPacman;
    xPacman2 = x - rPacman;
    yPacman1 = y - rPacman;
    yPacman2 = y - rPacman;
  } else if (pacman.direc == 'B') {
    xPacman1 = x - rPacman;
    xPacman2 = x + rPacman;
    yPacman1 = y + rPacman;
    yPacman2 = y + rPacman;
  }
  tft.fillCircle(x, y, rPacman, YELLOW);
  tft.fillTriangle(x, y, xPacman1, yPacman1, xPacman2, yPacman2, backgroundColor);
}

/* ------------------------------- */
/* Affiche un Fantome */
/* ------------------------------- */
void display_ghost(int x, int y) {
  tft.fillCircle(x, y-2, 7, RED);
  tft.fillTriangle(x, y, x-rGhost, y, x-rGhost, y+rGhost+2, RED);
  tft.fillTriangle(x, y, x+rGhost, y, x+rGhost, y+rGhost+2, RED);
  tft.fillTriangle(x-rGhost, y, x+rGhost, y, x, y+rGhost+2, RED);

  tft.fillCircle(x+xGhostEye1, y+yGhostEye, rGhostEyes, WHITE);
  tft.fillCircle(x+xGhostEye2, y+yGhostEye, rGhostEyes, WHITE);

  tft.drawLine(x+xGhostEye1+1, y+yGhostEye, x+xGhostEye1+1, y+yGhostEye+1, BLACK);
  tft.drawLine(x+xGhostEye2+1, y+yGhostEye, x+xGhostEye2+1, y+yGhostEye+1, BLACK);
}

/* ------------------------------- */
/* Affiche un carré pour caché le Pac-Man */
/* ------------------------------- */
void display_clear(int x, int y, int w, int l) {
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
  int color;
  // Losange de plus en plus petit
  for (int i = coteLosange; i > 1; i--) { 
    // Losange blanc du fond
    if (i == coteLosange) 
      color = WHITE;
    // Losange orange par dessus
    else if (i == coteLosange-1)
      color = ORANGE;
    // Losange jaune par dessus
    else if (i == coteLosange-2)
      color = YELLOW;
    // Losange blanc par dessus
    else
      color = WHITE;
    tft.fillTriangle(x-i, y, x, y-i, x, y+i, color);
    tft.fillTriangle(x+i, y, x, y-i, x, y+i, color);
  }
  tft.fillTriangle(x+centerStarX1, y+centerStarY1-1, x+centerStarX1-1, y+centerStarY1, x+centerStarX1+1, y+centerStarY1, color);
  tft.fillTriangle(x+centerStarX1, y+centerStarY1+1, x+centerStarX1-1, y+centerStarY1, x+centerStarX1+1, y+centerStarY1, color);
  
  tft.fillTriangle(x+centerStarX2, y+centerStarY2-1, x+centerStarX2-1, y+centerStarY2, x+centerStarX2+1, y+centerStarY2, color);
  tft.fillTriangle(x+centerStarX2, y+centerStarY2+1, x+centerStarX2-1, y+centerStarY2, x+centerStarX2+1, y+centerStarY2, color);
  
  tft.fillTriangle(x+centerStarX3, y+centerStarY3-1, x+centerStarX3-1, y+centerStarY3, x+centerStarX3+1, y+centerStarY3, color);
  tft.fillTriangle(x+centerStarX3, y+centerStarY3+1, x+centerStarX3-1, y+centerStarY3, x+centerStarX3+1, y+centerStarY3, color);
}

/* ------------------------------- */
/* Affiche le cadre du jeu */
/* ------------------------------- */
void display_outline(int x, int y) {
  tft.drawRect(x, y, ICONE, ICONE, BLACK);
}

/* ------------------------------- */
/* Affiche le logo du jeu */
/* ------------------------------- */
void display_game_pacman(int x, int y) {
  int backColor = LIGHTGRAY;  
  int contour = 2;
  tft.fillRect(x, y, ICONE, ICONE, backColor);  
  
  tft.setCursor(x+5, y+ICONE-20);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("PACMAN");

  tft.setCursor(x+6, y+ICONE-19);
  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.print("PACMAN");

  tft.fillCircle(x+(ICONE*3/9), y+(ICONE*3/8), rIconePacman, BLACK);
  tft.fillCircle(x+(ICONE*3/9), y+(ICONE*3/8), rIconePacman-2, YELLOW);

  tft.fillTriangle(x+(ICONE*3/9), y+(ICONE*3/8), x+(ICONE*3/9)+rIconePacman*cos(PI/4), y+(ICONE*3/8)-rIconePacman*sin(PI/4), x+(ICONE*3/9)+rIconePacman*cos(PI/4), y+(ICONE*3/8  )+rIconePacman*sin(PI/4), BLACK);
  tft.fillTriangle(x+(ICONE*3/9)+contour, y+(ICONE*3/8), x+(ICONE*3/9)+rIconePacman+contour, y+(ICONE*3/8)-rIconePacman, x+(ICONE*3/9)+rIconePacman+contour, y+(ICONE*3/8)+rIconePacman, backColor);

  display_scoringBalls(x+(ICONE*3/8)+rScoring*2, y+(ICONE*3/8));
  display_scoringBalls(x+(ICONE*3/8)+rScoring*5, y+(ICONE*3/8));
  display_invinsibilityBalls(x+(ICONE*3/8)+rScoring*8, y+(ICONE*3/8));
}

/* ------------------------------- */
/* Affiche le logo du jeu */
/* ------------------------------- */
void display_selectedGame(int x, int y, boolean isTheGame) {
  if (isTheGame) 
    tft.drawRect(x, y, ICONE, ICONE, BLUE);
  else 
    display_outline(x, y);
}

/* ------------------------------- */
/* Affiche la liste des jeux */
/* ------------------------------- */
void display_all_game(int x, int y) {
  oldSelectedGameX = selectedGameX;
  oldSelectedGameY = selectedGameY;
  selectedGameX += x;
  selectedGameY += y;
  if (selectedGameX < 0) 
    selectedGameX += SCREENWIDTH;
  if (selectedGameX >= SCREENWIDTH)
    selectedGameX -= SCREENWIDTH;
  if (selectedGameY < 0) 
    selectedGameY += SCREENHEIGHT;
  if (selectedGameY >= SCREENHEIGHT)
    selectedGameY -= SCREENHEIGHT;
  delay(200);
  for (int i = 0; i < SCREENWIDTH; i += ICONE) {
    for (int j = 0; j < SCREENHEIGHT; j += ICONE) {
      if (game[i+j] == "pacman")
        display_game_pacman(i*ICONE, j*ICONE);
      display_outline(i, j);     
    }
  }
  display_selectedGame(oldSelectedGameX, oldSelectedGameY, false);
  display_selectedGame(selectedGameX, selectedGameY, true);
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
  const int tempoKeepScore = 200;
  // Première colonne: Ton 
  // Deuxième colonne: Octave
  // Troisième colonne: Rythme
  int keepScore[5][3] = {
    DO,  4, 10,
    SOL, 4, 10,
    DO,  4, 10, 
    SOL, 4, 10,
    0,    0, 0
  };

  /* ------------------------------- */
  /* Son au ramassage de boule d'invincibilité */
  /* ------------------------------- */
  const int tempoKeepInvincib = 200;
  // Première colonne: Ton 
  // Deuxième colonne: Octave
  // Troisième colonne: Rythme
  int keepInvincib[9][3] = {
    DO,  4, 10,
    MI,  4, 10,
    SOL, 4, 10, 
    SI,  4, 10,
    RE,  5, 10,
    FA,  5, 10,
    LA,  5, 10,
    DO,  6, 10,
    0,   0, 0
  };

  /* ------------------------------- */
  /* Son à la mort de Pac-Man */
  /* ------------------------------- */
  const int tempoDeathPacman = 150;
  // Première colonne: Ton 
  // Deuxième colonne: Octave
  // Troisième colonne: Rythme
  int deathPacman[18][3] = {
    DO,  5, 10,
    MI,  5, 20,
    LA,  4, 10,
    DO,  5, 20,
    FA,  4, 10,
    LA,  4, 20,
    RE,  4, 10,
    FA,  4, 20,
    SI,  3, 10,
    RE,  4, 20,
    0,   0, 10,
    FA,  2, 10,
    SOL, 2, 10,
    SOL, 3, 10,
    FA,  2, 10,
    SOL, 2, 10,
    SOL, 3, 10,
    0,   0, 0
  };


/* ------------------------------- */
/* Retourne vrai si il y a une collision avec le terrain */
/* ------------------------------- */
bool isAlive(Entity pacman) {
  if (party.partyLifepoint > 0) {
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
/* Retourne vrai si il y a une collision avec une boule d'invinsibilité */
/* ------------------------------- */
bool collectableInvinc(int x, int y, Entity entity) {
  if (entity.type == 'G') {
    if (ground[y][x] == '&' && (x == entity.prevX && y == entity.prevY))
      return true;
    else
      return false;
  }
  if (ground[y][x] == '&' && (x == entity.actuX && y == entity.actuY)) {
    ground[y][x] = 'x';
    return true;
  } else {
    return false;
  }
}

/* ------------------------------- */
/* Initialise l'état d'invincibilité */
/* ------------------------------- */
void setInvincible(Entity &pacman, Entity &ghost) {
  pacman.invincibility = TIMEINVINC;
  ghost.invincibility = TIMEINVINC;
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
/* Modifie le temps restant de l'invincibilité et de fuite */
/* ------------------------------- */
void reduceInvincibility(Entity &pacman, Entity &ghost) {
  if (pacman.invincibility > 0 && ghost.invincibility > 0) {
    pacman.invincibility--;
    ghost.invincibility--;
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
    if (pacman.invincibility <= 0 && ghost.invincibility <= 0) {
      pacman.actuX = PACMANX;
      pacman.actuY = PACMANY;
      sounds(deathPacman, tempoDeathPacman, ROW_COUNT(deathPacman));
      party.partyLifepoint--;
      display_deleteLife();
    }
    // Place le Fantôme à sa position de départ
    ghost.actuX = GHOSTX;
    ghost.actuY = GHOSTY;
  } 

  // Si il y a une collision entre un Pac-Man et une boule de score, incrémente le score
  if (collectablePoint(pacman.actuX, pacman.actuY, pacman)) {
    party.partyScore++;
    sounds(keepScore, tempoKeepScore, ROW_COUNT(keepScore));
  }

  // Si il y a une collision entre un Pac-Man et une boule d'invincibilité, change son état et celui des fantômes
  if (collectableInvinc(pacman.actuX, pacman.actuY, pacman)) {
    setInvincible(pacman, ghost);
    sounds(keepInvincib, tempoKeepInvincib, ROW_COUNT(keepInvincib));
  }
  
  // Si il y a une collision avec le terrain, le Pac-Man retoure à la position précédente
  if (occuped(pacman.actuX, pacman.actuY)) { 
    pacman.actuY = pacman.prevY;
    pacman.actuX = pacman.prevX;
  } else { 
    if ((pacman.actuX != pacman.prevX || pacman.actuY != pacman.prevY)) {
      // Suppression de l'affichage du pacman à son ancienne position
      display_clear(pacman.prevX*BOX-(BOX/2), pacman.prevY*BOX+(BOX/2), BOX, BOX);
      // Ajout de l'affichage du pacman à sa nouvelle position
      display_pacman(pacman, GRAY);
    } 
  }
  
  // Si il y a une collision avec le terrain, le Fantome retoure à la position précédente
  if (occuped(ghost.actuX, ghost.actuY)) { 
    ghost.actuY = ghost.prevY;
    ghost.actuX = ghost.prevX;
  } else { 
    if (ghost.actuX != ghost.prevX || ghost.actuY != ghost.prevY) {
      // Suppression de l'affichage du Fantome à son ancienne position
      display_clear(ghost.prevX*BOX-(BOX/2), ghost.prevY*BOX+(BOX/2), BOX, BOX);
      if (ghost.prevX == pacman.actuX && ghost.prevY == pacman.actuY)
        display_pacman(pacman, GRAY);
      // Ajout de l'affichage de l'ancienne boule de score
      if (collectablePoint(ghost.prevX, ghost.prevY, ghost)) {
        display_scoringBalls((ghost.prevX)*BOX, (ghost.prevY+1)*BOX);
      }
      // Ajout de l'affichage de l'ancienne boule d'invinsibilité
      if (collectableInvinc(ghost.prevX, ghost.prevY, ghost)) {
        display_invinsibilityBalls((ghost.prevX)*BOX, (ghost.prevY+1)*BOX);
      }
      // Ajout de l'affichage du Fantome à sa nouvelle position
      display_ghost(ghost.actuX*BOX, ghost.actuY*BOX+BOX);
    } else if (pacman.invincibility <= 0 && ghost.invincibility <= 0) 
      // Ajout de l'affichage du Fantome à sa nouvelle position
      display_ghost(ghost.actuX*BOX, ghost.actuY*BOX+BOX);
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
void initGamePacman() {
  // Nombre de boule d'invinscibilité
  nbInvinc = 0;

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
    0
  };

  // Initialisation du Fantôme
  ghost = {
    'G',
    GHOSTX, GHOSTY,
    // Position hors du terrain car il n'a pas encore bougé 
    -GHOSTX, -GHOSTY, 
    'L',
    0
  };

  // Initialisation de la partie
  party = {
    0, // Nombre de minutes
    0, // Nombre de secondes
    0, // Nombre de millisecondes
    0, // Score actuel
    -1, // Score avant le score actuel
    0, // Score maximal de la partie, incrémenté à chaque ajout de boule de score
    3 // Point de vie du Pac-Man
  };

  // Appel des fonctions pour l'affichage
  tft.fillScreen(GRAY);
  display_ground(pacman, party);
  display_topMenu();
  display_topMenuLabel();
  display_topMenuLife(pacman);
}


/* ------------------------------- */
/* Lance le jeu Pac-Man */
/* ------------------------------- */
void play_pacman() {
  boolean menu = false;
  boolean start = true;
  while (true) {
    sounds(startPacman, tempoStartPacman, ROW_COUNT(startPacman)); // Son d'intro du Pac-Man
    initGamePacman(); // Initialise toute les variables
    
    // Tant que le Pac-Man a des vies et qu'il n'a pas le score maximal
    while (isAlive(pacman) && party.partyScore != party.partyMaxScore) {
      display_topMenuTime(party);
      display_topMenuScore(party);
      applyOccuped(pacman, ghost, party);
      testInput(pacman, ghost);
      reduceInvincibility(pacman, ghost);
    }
    
    // S'il n'a plus de vie, écran de GAMEOVER
    if (!isAlive(pacman))
      display_badEnd();
    // S'il a le score maximal, c'est à dire tout récuperer en plus d'avoir au moins une vie restante
    else if (party.partyScore == party.partyMaxScore)
      display_goodEnd();
    // Tant qu'il ne choisis pas de rejouer ou de quitter
    while (true) {
      if (digitalRead(27)) {
        delay(100);
        break;
      } else if (digitalRead(26) && !menu) {
        /*AFFICHER L'ECRAN D'ACCUEIL */
        tft.fillRect(0, 0, 320, 240, RED);
        menu = true;
      } else if (digitalRead(26) && menu) {
        start = false;
        break;
      }
    }
    if (menu && !start)
      break;
  }
}


/* ------------------------------- */
/* Setup */
/* ------------------------------- */
void setup() {
  Serial.begin(9600);
  tft.begin();
  tft.setRotation(LANDSCAPE);

  randomSeed(analogRead(0)); // Initialisation de la séquence aléatoire
  
  pinMode(28, INPUT); pinMode(24, INPUT); // Bas
  pinMode(29, INPUT); pinMode(23, INPUT); // Haut
  pinMode(30, INPUT); pinMode(22, INPUT); // Droite
  pinMode(31, INPUT); pinMode(25, INPUT); // Gauche
  pinMode(27, INPUT); // Bouton START (vert)
  pinMode(26, INPUT); // Bouton SELECT (rouge)
  pinMode(32, OUTPUT); // Haut-parleur
  display_all_game(0, 0);
}

/* ------------------------------- */
/* Loop */
/* ------------------------------- */
void loop() {
  // Tant que le jeu n'a pas été selectionné
  while (!digitalRead(27)) {
    if (digitalRead(28))
      display_all_game(0, ICONE);
    else if (digitalRead(29))
      display_all_game(0, -ICONE);
    else if (digitalRead(30))
      display_all_game(ICONE, 0);
    else if (digitalRead(31))
      display_all_game(-ICONE, 0);
    delay(100);
  }
  if (game[(selectedGameX/ICONE)+(selectedGameY/ICONE)] == "pacman") {
    play_pacman();
  }
  // Couleur de fond s'il n'y a pas tout les jeux
  tft.fillRect(0, 0, SCREENWIDTH, SCREENHEIGHT, LIGHTGRAY);
  display_all_game(0, 0);
}
