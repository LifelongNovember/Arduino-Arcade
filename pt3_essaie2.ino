#include "Adafruit_ILI9341.h"
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "math.h"

#define TFT_DC 9
#define TFT_CS 10

// Constante contenant le code décimal des couleurs
// Site pour convertir une couleur en code RGB565 : http://www.barth-dev.de/online/rgb565-color-picker/
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0
#define WHITE    0xFFFF
#define BROWN    0x8200
#define GRAY     0x31A6

// Rotation
#define PORTRAIT 0
#define LANDSCAPE 1

// Nombre de ligne d'un tableau
#define ROW_COUNT(array)    (sizeof(array) / sizeof(*array))
// Nombre de colonne d'un tableau
#define COLUMN_COUNT(array) (sizeof(array) / (sizeof(**array) * ROW_COUNT(array)))

// Taille d'une casse
#define BOX 20

// Taille d'écran
#define SCREENWIDTH 320 // Longueur
#define SCREENHEIGHT 240 // Largeur

// Position de départ du PacMan
#define FIRSTX 1
#define FIRSTY 1

// Définition du Pac-Man
struct PacMan {
  int actuX; // Coordonnée X actuelles
  int actuY; // Coordonnée Y actuelles
  int prevX; // Coordonnées X anciennes
  int prevY; // Coordonnée Y anciennes
  char direc; // Direction HAUT = H, BAS = B, GAUCHE = G, Droite = D
};

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

char ground[13][19] = {
  {'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'},
  {'x', 'o', 'o', 'o', 'o', 'x', 'o', 'o', 'o', 'o', 'o', 'x', 'o', 'o', 'x', 'o', 'o', 'o', 'x'},
  {'x', 'o', 'x', 'x', 'o', 'x', 'o', 'x', 'x', 'x', 'o', 'o', 'o', 'o', 'o', 'o', 'x', 'o', 'x'},
  {'x', 'o', 'o', 'o', 'o', 'x', 'o', 'o', 'o', 'x', 'x', 'x', 'x', 'o', 'x', 'o', 'o', 'o', 'x'},
  {'x', 'x', 'o', 'x', 'o', 'o', 'o', 'x', 'o', 'x', 'o', 'o', 'o', 'o', 'o', 'x', 'o', 'x', 'x'},
  {'x', 'o', 'o', 'o', 'o', 'x', 'x', 'x', 'o', 'x', 'o', 'x', 'o', 'x', 'o', 'o', 'o', 'x', 'x'},
  {'x', 'o', 'x', 'x', 'o', 'o', 'o', 'o', 'o', 'x', 'o', 'x', 'o', 'x', 'x', 'o', 'x', 'x', 'x'},
  {'x', 'o', 'o', 'o', 'o', 'x', 'x', 'x', 'o', 'x', 'o', 'x', 'o', 'x', 'o', 'o', 'o', 'x', 'x'},
  {'x', 'x', 'o', 'x', 'o', 'o', 'o', 'x', 'o', 'x', 'o', 'o', 'o', 'o', 'o', 'x', 'o', 'x', 'x'},
  {'x', 'o', 'o', 'o', 'o', 'x', 'o', 'o', 'o', 'x', 'x', 'x', 'x', 'o', 'x', 'o', 'o', 'o', 'x'},
  {'x', 'o', 'x', 'x', 'o', 'x', 'o', 'x', 'x', 'x', 'o', 'o', 'o', 'o', 'o', 'o', 'x', 'o', 'x'},
  {'x', 'o', 'o', 'o', 'o', 'x', 'o', 'o', 'o', 'o', 'o', 'x', 'o', 'o', 'x', 'o', 'o', 'o', 'x'},
  {'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'},
};

/*char ground[5][5] = {
  {'x', 'x', 'x', 'x', 'x'},
  {'x', 'o', 'o', 'o', 'x'},
  {'x', 'o', 'o', 'o', 'x'},
  {'x', 'o', 'o', 'o', 'x'},
  {'x', 'x', 'x', 'x', 'x'},
};*/

// Booléen de test pour le joystick
bool exec = false; 

struct PacMan pacman = {
  FIRSTX, FIRSTY,
  // Position hors du terrain car il n'a pas encore bougé 
  -FIRSTX, -FIRSTY, 
  'D'
};

// Points pour le dessin à l'aide des primitives de la librairie graphique
/* --------------------------------------*/
/* Coordonnées du PacMan pour l'affichage*/
/* --------------------------------------*/
int r = 7 ;
double a = PI / 4;
int x1, x2, y1, y2, xR, yR;
/* --------------------------------------*/

/* ---------------------------------------*/
/* Coordonnées des Bricks pour l'affichage*/
/* ---------------------------------------*/
int wightBrick1 = 9;
int heightBrick = 4;
int wightBrick2 = 5;
int wightBrick3 = 1;
/* ---------------------------------------*/

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
  
void display_pacman(PacMan &pacman) {
  int x = (pacman.actuX*BOX)+(BOX/2);
  int y = (pacman.actuY*BOX)+(BOX/2);
  int x1 = x + r * cos(a);
  int x2 = x + r * cos(a);
  int y1 = y - r * sin(a);
  int y2 = y + r * sin(a);
  int xR = x1 + 1;
  int yR = y1 + 1;
  int w = (x + r) - x1;
  int h = y2 - y1 + 2;
  int histoW = w;
  int tmp = h;
  if (pacman.direc == 'D') {
    x1 = x + r * cos(a);
    x2 = x + r * cos(a);
    y1 = y - r * sin(a);
    y2 = y + r * sin(a);
    xR = x1 + 1;
    yR = y1 + 1;
  } else if (pacman.direc == 'G') {
    x1 = x - r * cos(a)-1;
    x2 = x - r * cos(a)-1;
    y1 = y - r * sin(a);
    y2 = y + r * sin(a);
    xR = x1 - 1;
    yR = y1 - 1;
  } else if (pacman.direc == 'H') {
    x1 = x + r * cos(a);
    x2 = x - r * cos(a)-1;
    y1 = y - r * sin(a);
    y2 = y - r * sin(a);
    xR = x2;
    yR = y1 - histoW;
    tmp = h;
    h = w;
    w = tmp;
  } else if (pacman.direc == 'B') {
    x1 = x - r * cos(a);
    x2 = x + r * cos(a);
    y1 = y + r * sin(a);
    y2 = y + r * sin(a);
    xR = x1;
    yR = y1;
    tmp = h;
    h = w;
    w = tmp;
  }
  
  tft.fillCircle(x, y, r, YELLOW);
  tft.fillTriangle(x, y, x1, y1, x2, y2, GRAY);
  tft.fillRect(xR, yR, w, h, GRAY);
}

// Affiche un rectangle pour cacher le pacman
void display_brick(int x, int y, int w, int l) {
  tft.fillRect(x, y, w, l, GRAY);
}

// Retourne vrai si il y a collision avec le terrain, sinon faux
bool occuped(int x, int y) {
  if (ground[y][x] == 'x') {
    return true;
  } else {
    return false;
  }
}

// Affiche le PacMan à la bonne position s'il n'y a pas de collision
void applyOccuped(PacMan &pacman) {
  // Si il y a une collision avec le terrain, retoure à la position précédente
  if (occuped(pacman.actuX, pacman.actuY) == true) { 
    pacman.actuY = pacman.prevY;
    pacman.actuX = pacman.prevX;
  } else { 
    if ((pacman.actuX != pacman.prevX || pacman.actuY != pacman.prevY) && exec == false) {
      // Suppression de l'affichage du pacman à son ancienne position
      display_brick(pacman.prevX*BOX, pacman.prevY*BOX, BOX, BOX);
      // Ajout de l'affichage du pacman à sa nouvelle position
      display_pacman(pacman);
    } 
  }
}

// Modifie la position du PacMan suivant la position du joystick
void testInput(PacMan &pacman) {
  if (exec == false) {
    pacman.prevY = pacman.actuY;
    pacman.prevX = pacman.actuX;
    if (digitalRead(28) == true || digitalRead(24) == true) {
      pacman.direc = 'B';
      pacman.actuY++;
      exec = true;
      delay(100);
    }
    else if (digitalRead(29) == true || digitalRead(23) == true) {
      pacman.direc = 'H';
      pacman.actuY--;
      exec = true;
      delay(100);
    }
    else if (digitalRead(30) == true || digitalRead(22) == true) {
      pacman.direc = 'D';
      pacman.actuX++;
      exec = true;
      delay(100);
    }
    else if (digitalRead(31) == true || digitalRead(25) == true) {
      pacman.direc = 'G';
      pacman.actuX--;
      exec = true;
      delay(100);
    }
  }
  if (digitalRead(28) == false &&
      digitalRead(29) == false &&
      digitalRead(30) == false &&
      digitalRead(31) == false ) {
      exec = false; 
  }
}

void display_ground(PacMan &pacman) {
  // Vrai si on sort par la droite
  bool exceedRight = false;
  // vrai si on sort par le bas
  bool exceedBottom = false;
  
  // Affiche le terrain sans sortir des limites de l'afficheur
  for (int i = 0; i < ROW_COUNT(ground) && exceedBottom == false; i++) {
    exceedRight = false;
    /*if (BOX*(i+1) >= SCREENHEIGHT)
      exceedBottom = true;*/
    for (int j = 0; j < COLUMN_COUNT(ground) && exceedRight == false; j++) {
      if (ground[i][j] == 'x')
        display_brick(j*BOX, i*BOX, BOX, BOX, BROWN);
      /*if (BOX*(j+1) >= SCREENWIDTH)
        exceedRight = true;*/
    }
  }
}

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
}

void loop() {
  applyOccuped(pacman);
  testInput(pacman);
}
