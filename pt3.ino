#include "Adafruit_ILI9341.h"
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "math.h"

#define TFT_DC 9
#define TFT_CS 10

// Constante contenant le code décimal des couleurs
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0
#define WHITE    0xFFFF

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

// Définition du Pac-Man
struct PacMan {
  int actuX; // Coordonnée X actuelles
  int actuY; // Coordonnée Y actuelles
  int prevX; // Coordonnées X anciennes
  int prevY; // Coordonnée Y anciennes
  char direc; // Direction HAUT = H, BAS = B, GAUCHE = G, Droite = D
};

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

char ground[13][20] = {
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

bool exec = false; // Booléen de test pour le joystick
// Coordonnée de test des déplacements
int x = 100;
int y = 100;

struct PacMan pacman = {
  20, 20, -1, -1, 'D'
};

// Affiche un rectangle 
void display_brick(int x, int y, int w, int l, int color) {
  tft.fillRect(x, y, w, l, color);
}

// Retourne vrai si il y a collision, sinon faux
bool occuped(int x, int y) {
  if (ground[y/BOX][x/BOX] == 'x') {
    return true;
  } else {
    return false;
  }
}

// Affiche le PacMan à la bonne position s'il n'y a pas de collision
void applyOccuped(PacMan &pacman) {
  if (occuped(pacman.actuX, pacman.actuY) == true) {
  pacman.actuY = pacman.prevY;
  pacman.actuX = pacman.prevX;
  } else {
    display_brick(pacman.actuX, pacman.actuY, BOX, BOX, BLUE);
  }
}

// Modifie la position du PacMan suivant la position du joystick
void testInput(PacMan &pacman) {
  if (exec == false) {
    pacman.prevY = pacman.actuY;
    pacman.prevX = pacman.actuX;
    if (digitalRead(28) == true || digitalRead(23) == true) {
      pacman.actuY = pacman.actuY + BOX;
      exec = true;
      delay(100);
    }
    else if (digitalRead(29) == true || digitalRead(22) == true) {
      pacman.actuY = pacman.actuY - BOX;
      exec = true;
      delay(100);
    }
    else if (digitalRead(30) == true || digitalRead(25) == true) {
      pacman.actuX = pacman.actuX + BOX;
      exec = true;
      delay(100);
    }
    else if (digitalRead(31) == true || digitalRead(24) == true) {
      pacman.actuX = pacman.actuX - BOX;
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

void setup() {

  Serial.begin(9600);
  tft.begin();
  tft.fillScreen(BLACK);
  tft.setRotation(LANDSCAPE);
  pinMode(28, INPUT); pinMode(24, INPUT); // Bas
  pinMode(29, INPUT); pinMode(25, INPUT); // Haut
  pinMode(30, INPUT); pinMode(22, INPUT); // Droite
  pinMode(31, INPUT); pinMode(23, INPUT); // Gauche

  // Vrai si on sort par la droite
  bool exceedRight = false;
  // vrai si on sort par le bas
  bool exceedBottom = false;
  
  // Affiche le terrain sans sortir des limites de l'afficheur
  for (int i = 0; i < ROW_COUNT(ground) && exceedBottom == false; i++) {
    exceedRight = false;
    if (BOX*(i+1) >= SCREENHEIGHT)
      exceedBottom = true;
    for (int j = 0; j < COLUMN_COUNT(ground) && exceedRight == false; j++) {
      if (ground[i][j] == 'x')
        display_brick(j*BOX, i*BOX, BOX, BOX, RED);
      if (BOX*(j+1) >= SCREENWIDTH)
        exceedRight = true;
    }
  }
  
}

void loop() {
  testInput(pacman);
  applyOccuped(pacman);
  
}

// Dimension de l'écran en pixel : 
// 240 px de haut
// 320px de long
