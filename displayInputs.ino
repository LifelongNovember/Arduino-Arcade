#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

// For the Adafruit shield, these are the default.
#define TFT_DC 9
#define TFT_CS 10
// Inputs
#define STICK 0
#define DOWNLEFT 1
#define DOWN 2
#define DOWNRIGHT 3
#define LEFT 4
#define RIGHT 6
#define UPLEFT 7
#define UP 8
#define UPRIGHT 9
#define REDB 1
#define GREENB 2
#define BLACKB 3
#define NONE 5
// Nombre d'inputs qu'il est possible d'entrer simultanément
#define NBINPUTS 4
// Couleurs
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


// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// Contiendra les inputs entres.
int inputPressed[NBINPUTS];
// Est vrai si le stick est a la position neutre
bool neutralStick;

void setup() {
  
  Serial.begin(9600);
  tft.begin();

  // Initialisation d'inputPressed
  resetInputs();
  // Initialisation de l'affichage (dessin du setup sans aucun input entre)
  tft.setRotation(LANDSCAPE);
  tft.fillScreen(WHITE);

  drawDL();
  drawD();
  drawDR();
  drawL();
  drawR();
  drawUL();
  drawU();
  drawUR();

  drawRB();
  drawGB();
  drawBB();

  drawNeutral();

  // Test de chacune des positions du stick et des boutons
  registerInput(STICK,UP);
  fillInputs();
  delay(1500);
  resetInputs();
  resetDisplay();

  registerInput(STICK,UPRIGHT);
  fillInputs();
  delay(1500);
  resetInputs();
  resetDisplay();

  registerInput(STICK,RIGHT);
  fillInputs();
  delay(1500);
  resetInputs();
  resetDisplay();

  registerInput(STICK,DOWNRIGHT);
  fillInputs();
  delay(1500);
  resetInputs();
  resetDisplay();

  registerInput(STICK,DOWN);
  fillInputs();
  delay(1500);
  resetInputs();
  resetDisplay();

  registerInput(STICK,DOWNLEFT);
  fillInputs();
  delay(1500);
  resetInputs();
  resetDisplay();

  registerInput(STICK,LEFT);
  fillInputs();
  delay(1500);
  resetInputs();
  resetDisplay();

  registerInput(STICK,UPLEFT);
  fillInputs();
  delay(1500);
  resetInputs();
  resetDisplay();

  fillInputs();
  delay(1500);
  resetInputs();
  resetDisplay();

  registerInput(REDB,1);
  fillInputs();
  delay(1500);
  resetInputs();
  resetDisplay();

  registerInput(GREENB,1);
  fillInputs();
  delay(1500);
  resetInputs();
  resetDisplay();

  registerInput(BLACKB,1);
  fillInputs();
  delay(1500);
  resetInputs();
  resetDisplay();
}

void loop() {
  // Inputs aleatoires (pour le fun)
  // Generation d'un nombre aleatoire en lisant une broche inutilisee
  randomSeed(analogRead(13));
  // Enregistrement d'inputs aléatoires
  registerInput(STICK,(int) random(1,10));
  registerInput(REDB,(int) random(2));
  registerInput(GREENB,(int) random(2));
  registerInput(BLACKB,(int) random(2));
  // Dessin des inputs choisis
  fillInputs();
  // Affichage de 1,5 sec
  delay(1500);
  // Reinitialisation d'inputPressed
  resetInputs();
  // Reinitialisation de l'affichage
  resetDisplay();
}


// Fonctions permettant de tracer les fleches et cercles vides
void drawDL() {
  tft.drawTriangle(64,151,71,158,53,176,BLACK);
  tft.drawTriangle(53,176,46,169,64,151,BLACK);
  tft.drawTriangle(39,162,60,183,39,183,BLACK);
  tft.drawLine(64,151,53,176,WHITE);
  tft.drawLine(47,170,52,175,WHITE);
  tft.drawPixel(64,151,BLACK);
  tft.drawPixel(53,176,BLACK);
}

void drawD() {
  tft.drawRect(79,166,11,26,BLACK);
  tft.drawLine(69,191,100,191,BLACK);
  tft.drawLine(100,191,85,206,BLACK);
  tft.drawLine(84,206,69,191,BLACK);
  tft.drawLine(80,191,88,191,WHITE);
}
void drawDR() {
  tft.drawTriangle(96,159,103,152,114,177,BLACK);
  tft.drawTriangle(103,152,114,177,121,170,BLACK);
  tft.drawTriangle(128,163,128,184,107,184,BLACK);
  tft.drawLine(120,171,115,176,WHITE);
  tft.drawLine(103,152,114,177,WHITE);
  tft.drawPixel(103,152,BLACK);
  tft.drawPixel(114,177,BLACK);
}

void drawL() {
  tft.drawRect(32,134,26,11,BLACK);
  tft.drawLine(32,124,32,155,BLACK);
  tft.drawLine(32,155,17,140,BLACK);
  tft.drawLine(17,139,32,124,BLACK);
  tft.drawLine(32,143,32,135,WHITE);
}

void drawR() {
  tft.drawRect(110,135,26,11,BLACK);
  tft.drawLine(135,124,135,155,BLACK);
  tft.drawLine(135,155,150,140,BLACK);
  tft.drawLine(150,139,135,124,BLACK);
  tft.drawLine(135,136,135,144,WHITE);
}

void drawUL() {
  tft.drawTriangle(64,127,53,102,71,120,BLACK);
  tft.drawTriangle(46,109,53,102,64,127,BLACK);
  tft.drawTriangle(39,116,39,95,60,95,BLACK);
  tft.drawLine(64,127,53,102,WHITE);
  tft.drawLine(47,108,52,103,WHITE);
  tft.drawPixel(64,127,BLACK);
  tft.drawPixel(53,102,BLACK);
}
void drawU() {
  tft.drawRect(78,88,11,26,BLACK);
  tft.drawLine(67,88,98,88,BLACK);
  tft.drawLine(98,88,83,73,BLACK);
  tft.drawLine(82,73,67,88,BLACK);
  tft.drawLine(79,88,87,88,WHITE);
}
void drawUR() {
  tft.drawTriangle(96,121,121,110,103,128,BLACK);
  tft.drawTriangle(96,121,121,110,114,103,BLACK);
  tft.drawTriangle(107,96,128,96,128,117,BLACK);
  tft.drawLine(115,104,120,109,WHITE);
  tft.drawLine(96,121,121,110,WHITE);
  tft.drawPixel(96,121,BLACK);
  tft.drawPixel(121,110,BLACK);
}
void drawRB() {
  tft.drawCircle(185,140,15,BLACK);
}
void drawGB() {
  tft.drawCircle(235,140,15,BLACK);
}
void drawBB() {
  tft.drawCircle(285,140,15,BLACK);
}
void drawNeutral() {
  tft.drawCircle(84,140,15,BLACK);  
}

// Fonctions permettant de remplir les fleches et cercles
void fillDL() {
  tft.fillTriangle(64,151,71,158,53,176,BLUE);
  tft.fillTriangle(53,176,46,169,64,151,BLUE);
  tft.fillTriangle(39,162,60,183,39,183,BLUE);
}

void fillD() {
  tft.fillRect(79,166,11,26,BLUE);
  tft.fillTriangle(69,191,84,191,84,206,BLUE);
  tft.fillTriangle(85,191,85,206,100,191,BLUE);
}

void fillDR() {
  tft.fillTriangle(96,159,103,152,114,177,BLUE);
  tft.fillTriangle(103,152,114,177,121,170,BLUE);
  tft.fillTriangle(128,163,128,184,107,184,BLUE);
}

void fillL() {
  tft.fillRect(32,134,26,11,BLUE);
  tft.fillTriangle(32,155,32,140,17,140,BLUE);
  tft.fillTriangle(17,139,32,139,32,124,BLUE);
}

void fillR() {
  tft.fillRect(110,135,26,11,BLUE);
  tft.fillTriangle(135,155,150,140,135,140,BLUE);
  tft.fillTriangle(135,124,150,139,135,139,BLUE);
}

void fillUL() {
  tft.fillTriangle(64,127,53,102,71,120,BLUE);
  tft.fillTriangle(46,109,53,102,64,127,BLUE);
  tft.fillTriangle(39,116,39,95,60,95,BLUE);
}

void fillU() {
  tft.fillRect(78,88,11,26,BLUE);
  tft.fillTriangle(67,88,82,88,82,73,BLUE);
  tft.fillTriangle(83,73,83,88,98,88,BLUE);
}

void fillUR() {
  tft.fillTriangle(96,121,121,110,103,128,BLUE);
  tft.fillTriangle(96,121,121,110,114,103,BLUE);
  tft.fillTriangle(107,96,128,96,128,117,BLUE);
}

void fillRB() {
  tft.fillCircle(185,140,15,RED);
}

void fillGB() {
  tft.fillCircle(235,140,15,GREEN);
}

void fillBB() {
  tft.fillCircle(285,140,15,BLACK);
}

void fillNeutral() {
  tft.fillCircle(84,140,15,BLUE);
}

// Initialisation de l'afficheur et affichage du setup
void resetDisplay() {

  // On ecrase les formes existantes plutot que de remplir l'ecran de blanc
  // pour un meilleur refresh rate
  tft.fillTriangle(64,151,71,158,53,176,WHITE);
  tft.fillTriangle(53,176,46,169,64,151,WHITE);
  tft.fillTriangle(39,162,60,183,39,183,WHITE);
  tft.fillRect(79,166,11,26,WHITE);
  tft.fillTriangle(69,191,84,191,84,206,WHITE);
  tft.fillTriangle(85,191,85,206,100,191,WHITE);
  tft.fillTriangle(96,159,103,152,114,177,WHITE);
  tft.fillTriangle(103,152,114,177,121,170,WHITE);
  tft.fillTriangle(128,163,128,184,107,184,WHITE);
  tft.fillRect(32,134,26,11,WHITE);
  tft.fillTriangle(32,155,32,140,17,140,WHITE);
  tft.fillTriangle(17,139,32,139,32,124,WHITE);
  tft.fillRect(110,135,26,11,WHITE);
  tft.fillTriangle(135,155,150,140,135,140,WHITE);
  tft.fillTriangle(135,124,150,139,135,139,WHITE);
  tft.fillTriangle(64,127,53,102,71,120,WHITE);
  tft.fillTriangle(46,109,53,102,64,127,WHITE);
  tft.fillTriangle(39,116,39,95,60,95,WHITE);
  tft.fillRect(78,88,11,26,WHITE);
  tft.fillTriangle(67,88,82,88,82,73,WHITE);
  tft.fillTriangle(83,73,83,88,98,88,WHITE);
  tft.fillTriangle(96,121,121,110,103,128,WHITE);
  tft.fillTriangle(96,121,121,110,114,103,WHITE);
  tft.fillTriangle(107,96,128,96,128,117,WHITE);
  tft.fillCircle(185,140,15,WHITE);
  tft.fillCircle(235,140,15,WHITE);
  tft.fillCircle(285,140,15,WHITE);
  tft.fillCircle(84,140,15,WHITE);

  drawDL();
  drawD();
  drawDR();
  drawL();
  drawR();
  drawUL();
  drawU();
  drawUR();

  drawRB();
  drawGB();
  drawBB();

  drawNeutral();
}

// Initialisation d'inputPressed
void resetInputs() {
  inputPressed[0] = NONE;
  inputPressed[1] = 0;
  inputPressed[2] = 0;
  inputPressed[3] = 0;
  neutralStick = true;
}

// Ajout d'un input a inputPressed
void registerInput(int type, int val) {
  if(type >= NBINPUTS) {
    Serial.println("inputPressed overflow");
  } else {
    inputPressed[type] = val;
  }
}

// Remplissage des inputs que contient inputPressed
void fillInputs() {
  // Le stick ne peut s'oriention que vers une seule direction à la fois...
  switch(inputPressed[STICK]) {
      case NONE : break;
      case DOWNLEFT : fillDL(); neutralStick = false; break;
      case DOWN : fillD(); neutralStick = false; break;
      case DOWNRIGHT : fillDR(); neutralStick = false; break;
      case LEFT : fillL(); neutralStick = false; break;
      case RIGHT : fillR(); neutralStick = false; break;
      case UPLEFT : fillUL(); neutralStick = false; break;
      case UP : fillU(); neutralStick = false; break;
      case UPRIGHT : fillUR(); neutralStick = false; break;
  }
  // ... alors qu'on peut appouyer sur tous les boutons en même temps.
  if(inputPressed[REDB]) fillRB();
  if(inputPressed[GREENB]) fillGB();
  if(inputPressed[BLACKB]) fillBB();
  if(neutralStick) fillNeutral();
}
