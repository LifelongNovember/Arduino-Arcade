/* ------------------------------- */
/*    Imports des bibliothèques    */
/* ------------------------------- */
#include "Adafruit_ILI9341.h"
#include "Adafruit_GFX.h"
#include "SPI.h"
#include "math.h"

/* ------------------------------- */
/*  */
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
#define GREEN     0x07E0
#define CYAN      0x07FF
#define MAGENTA   0xF81F
#define YELLOW    0xFFE0
#define WHITE     0xFFFF
#define BROWN     0x8200
#define GRAY      0x31A6
#define LIGHTGRAY 0xB596
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
#define FIRSTX 2
/* Position en ordonnée (y) de départ du Pac-Man */
#define FIRSTY 1

/* ------------------------------- */
/* Position en abscice (x) de départ du Fantome */
#define GHOSTX 4
/* Position en ordonnée (y) de départ du Fantome */
#define GHOSTY 1

/* ------------------------------- */
/* Taille d'une case en pixel sur l'écran */
/* ------------------------------- */
#define DELAY 100

/* ------------------------------- */
/* Première position du Pac-Man symbolisant les vies */
/* ------------------------------- */
#define FIRSTLIFE 2

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
};

/* ------------------------------- */
/* Nombre de vie du Pac-Man */
/* ------------------------------- */
int lifepoint = 3;

/* ------------------------------- */
/* Définition d'une partie */
/* ------------------------------- */
struct Party {
  int partyTimeMin; // Champ des minutes pour notre temps
  int partyTimeSec; // Champ des secondes pour notre temps
  int partyTimeMil; // Champ des millisecondes pour notre temps
  int partyScore; // Score totale recolté
  int partyOldScore; // Score juste avant partyScore
};

/* ------------------------------- */
/*  */
/* ------------------------------- */
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);


/* ------------------------------- */
/* Terrain de jeu */
/* ------------------------------- */
char ground[13][19] = {
  {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'},
  {'#', 'o', 'o', 'o', 'o', '#', 'o', 'o', 'o', 'o', 'o', '#', 'o', 'o', '#', 'o', '#'},
  {'#', 'o', '#', '#', 'o', '#', 'o', '#', '#', '#', 'o', 'o', 'o', 'o', 'o', 'o', '#'},
  {'#', 'o', 'o', 'o', 'o', '#', 'o', 'o', 'o', '#', '#', '#', '#', 'o', '#', 'o', '#'},
  {'#', '#', 'o', '#', 'o', 'o', 'o', '#', 'o', '#', 'o', 'o', 'o', 'o', 'o', '#', '#'},
  {'#', 'o', 'o', 'o', 'o', '#', '#', '#', 'o', '#', 'o', '#', 'o', '#', 'o', 'o', '#'},
  {'#', 'o', '#', '#', 'o', 'o', 'o', 'o', 'o', '#', 'o', '#', 'o', '#', '#', 'o', '#'},
  {'#', 'o', 'o', 'o', 'o', '#', '#', '#', 'o', '#', 'o', '#', 'o', '#', 'o', 'o', '#'},
  {'#', '#', 'o', '#', 'o', 'o', 'o', '#', 'o', '#', 'o', 'o', 'o', 'o', 'o', '#', '#'},
  {'#', 'o', 'o', 'o', 'o', '#', 'o', 'o', 'o', '#', '#', '#', '#', 'o', '#', 'o', '#'},
  //{'#', 'o', '#', '#', 'o', '#', 'o', '#', '#', '#', 'o', 'o', 'o', 'o', 'o', 'o', '#'},
  {'#', 'o', 'o', 'o', 'o', '#', 'o', 'o', 'o', 'o', 'o', '#', 'o', 'o', '#', 'o', '#'},
  {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'}, 
};

/*char ground[5][5] = {
  {'#', '#', '#', '#', '#'},
  {'#', 'o', 'o', 'o', '#'},
  {'#', 'o', 'o', 'o', '#'},
  {'#', 'o', 'o', 'o', '#'},
  {'#', '#', '#', '#', '#'},
};*/

/* ------------------------------- */
/* Initialisation du Pac-Man */
/* ------------------------------- */
struct Entity pacman = {
  'P',
  FIRSTX, FIRSTY,
  // Position hors du terrain car il n'a pas encore bougé 
  -FIRSTX, -FIRSTY, 
  'R'
};

/* ------------------------------- */
/* Initialisation du Fantome */
/* ------------------------------- */
struct Entity ghost = {
  'G',
  GHOSTX, GHOSTY,
  // Position hors du terrain car il n'a pas encore bougé 
  -GHOSTX, -GHOSTY, 
  'L'
};

/* ------------------------------- */
/* Initialisation de la party */
/* ------------------------------- */
struct Party party = {
  0, // Nombre de minutes
  0, // Nombre de secondes
  0, // Nombre de millisecondes
  0, // Score actuel
  -1 // Score avant le score actuel
};


/* ------------------------------- */
/* Coordonnées pour faire les designs graphiques */
/* ------------------------------- */
  
  /* ------------------------------- */
  /* Coordonnées du PacMan pour l'affichage */
  /* ------------------------------- */
  int r = 7 ;
  double a = PI / 4;
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
  int rInvincib = 5; // Rayon du cercle principal
  int r2Invincib = 2; // Rayon des cercles de chaque côtés
  

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
void display_ground(Entity pacman) {
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
      else if (ground[i][j] == 'o' && !collectablePoint(j, i, pacman))
        display_scoringBalls(j*BOX, (i+1)*BOX);
      /*if (BOX*(j+1) >= SCREENWIDTH)
        exceedRight = true;*/
    }
  }
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
  if (!digitalRead(28) && !digitalRead(29) && !digitalRead(30) && !digitalRead(31))
    delay(DELAY);
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
  int x1Center = x - rInvincib;
  int x2Center = x + rInvincib;
  int y1Center = y - rInvincib;
  int y2Center = y + rInvincib;

  int x1Line = x - rInvincib * cos(a);
  int x2Line = x + rInvincib * cos(a);
  int y1Line = y - rInvincib * sin(a);
  int y2Line = y + rInvincib * sin(a);
  
  tft.fillCircle(x, y, rInvincib, GRAY);
  
  tft.fillCircle(x1Center, y, r2Invincib, BLUE);
  tft.fillCircle(x, y1Center, r2Invincib, RED);
  tft.fillCircle(x2Center, y, r2Invincib, GREEN);
  tft.fillCircle(x, y2Center, r2Invincib, YELLOW);
  
  tft.drawLine(x1Line, y1Line, x2Line, y2Line, LIGHTGRAY);
  tft.drawLine(x1Line, y2Line, x2Line, y1Line, LIGHTGRAY);
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

  // Si il y a une collision entre un Pac-Man et un Fantome, place le Pac-Man à sa position de départ et lui retire une vie
  if (entityCollision(pacman, ghost)) {
    pacman.actuX = FIRSTX;
    pacman.actuY = FIRSTY;
    lifepoint--;
    display_deleteLife();
  } 

  // Si il y a une collision entre un Pac-Man et une boule de score, incrémente le score
  if (collectablePoint(pacman.actuX, pacman.actuY, pacman)) {
    party.partyScore++;
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
    delay(DELAY);
  } else if (digitalRead(30) || (digitalRead(30) && digitalRead(28)) || (digitalRead(30) && digitalRead(29))) {
    pacman.direc = 'R';
    pacman.actuX++;
    delay(DELAY);
  } else if (digitalRead(29)) {
    pacman.direc = 'T';
    pacman.actuY--;
    delay(DELAY);
  } else if (digitalRead(28)) {
    pacman.direc = 'B';
    pacman.actuY++;
    delay(DELAY);
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
/* Setup */
/* ------------------------------- */
void setup() {
  Serial.begin(9600);
  tft.begin();
  tft.fillScreen(GRAY);
  tft.setRotation(LANDSCAPE);
  pinMode(28, INPUT); pinMode(24, INPUT); // Bas
  pinMode(29, INPUT); pinMode(23, INPUT); // Haut
  pinMode(30, INPUT); pinMode(22, INPUT); // Droite
  pinMode(31, INPUT); pinMode(25, INPUT); // Gauche
  display_ground(pacman);
  display_topMenu();
  display_topMenuLabel();
  display_topMenuLife(pacman);
  //display_invinsibilityBalls();
}

/* ------------------------------- */
/* Loop */
/* ------------------------------- */
void loop() {
  display_topMenuTime(party);
  display_topMenuScore(party);
  applyOccuped(pacman, ghost, party);
  testInput(pacman, ghost);
}

// tft.dear(COLOR) efface l'écran avec la couleur voulue
// tft.RGB(r, g, b) retourne une couleur au format RGB
// tft.drawPixel(x, y, COLOR) affiche un pixel
