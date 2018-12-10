
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
#define BOX 40

// Taille d'écran
#define SCREENWIDTH 320 // Longueur
#define SCREENHEIGHT 240 // Largeur

// Définition du Pac-Man
struct PacMan {
  int posX; // Coordonnée X
  int posY; // Coordonnée Y
  int direc; // Direction 
};

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// Points pour le dessin à l'aide des primitives de la librairie graphique
/*int x = 130;
int y = 90;
int r = 20;
double a = PI / 4;
int x1 = x + r * cos(a);
int x2 = x + r * cos(a);
int y1 = y - r * sin(a);
int y2 = y + r * sin(a);
int xR = x1 + 1;
int yR = y1 + 1;
int w = (x + r) - x1;
int h  = y2 - y1;*/

// Map du jeu
/*char ground[6][5] = {
  {'x', 'x', 'x', 'x', 'x'}, 
  {'x', 'x', 'x', 'x', 'x'},  
  {'x', 'o', 'o', 'o', 'x'}, 
  {'x', 'o', 'o', 'x', 'x'}, 
  {'x', 'o', 'o', 'x', 'x'}, 
  {'x', 'x', 'x', 'x', 'x'}, 
};*/


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

bool exec = false;


void setup() {

  Serial.begin(9600);
  tft.begin();
  tft.fillScreen(BLACK);
  tft.setRotation(LANDSCAPE);
  pinMode(28, INPUT); // Haut
  pinMode(29, INPUT); // Bas
  pinMode(30, INPUT); // Gauche
  pinMode(31, INPUT); // Droite

}

void loop() {
  /*bool exceedRight = false;
  bool exceedBottom = false; 
  for (int i = 0; i < ROW_COUNT(ground) && exceedBottom == false; i++) {
    exceedRight = false;
    if (BOX*(i+1) >= SCREENHEIGHT)
      exceedBottom = true;
    for (int j = 0; j < COLUMN_COUNT(ground) && exceedRight == false; j++) {
      if (ground[i][j] == 'x')
        display_brick(j*BOX, i*BOX, BOX, BOX);
      if (BOX*(j+1) >= SCREENWIDTH)
        exceedRight = true;
    }
  }*/
  if (exec == false) {
    if (digitalRead(28) == true) {
      tft.println("HAUT !!");
      exec = true;
    }
    else if (digitalRead(29) == true) {
      tft.println("BAS !!");
      exec = true;
    }
    else if (digitalRead(30) == true) {
      tft.println("GAUCHE !!");
      exec = true;
    }
    else if (digitalRead(31) == true) {
      tft.println("DROITE !!");
      exec = true;
    }
  }
  if (digitalRead(28) == false &&
              digitalRead(29) == false &&
              digitalRead(30) == false &&
              digitalRead(31) == false ) {
      exec = false;   
  }
}

void display_brick(int x, int y, int w, int l) {
  tft.drawRect(x, y, w, l, RED);
}

// Dimension de l'écran en pixel : 
// 240 px de haut
// 320px de long
