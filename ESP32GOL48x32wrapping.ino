/*******************************************************************
 *                                                                 *
    An ESP32 port of Conway's Game of Life sketch for Arduino by Aerodynamics
    Project here: https://www.hackster.io/aerodynamics/arduino-led-matrix-game-of-life-093f06
    
    Based on the great work of Brian Lough and Leonid Rusnac (@lrusnac github)
    https://www.youtube.com/channel/UCezJOfu7OtqGzd5xrP3q6WA
    
   
    Edited by Emily Velasco (twitter.com/MLE_Online)
    Edge wrapping added by Ted Yapo (twitter/com/tedyapo)
 *******************************************************************/

// ----------------------------
// Standard Libraries - Already Installed if you have ESP8266 set up
// ----------------------------

#include <Ticker.h>

#include <PxMatrix.h>
// The library for controlling the LED Matrix
// Needs to be manually downloaded and installed
// https://github.com/2dom/PxMatrix

Ticker display_ticker;

// Pins for LED MATRIX
#define P_LAT 22
#define P_A 19
#define P_B 23
#define P_C 18
#define P_D 5
#define P_E 15
#define P_OE 2

bool WORLD[48][32]; // Creation of the world
bool WORLD2[48][32]; // Creation of the world
int step_GOL; //used to know the generation
//uint16_t world_frame[16][32];

 PxMATRIX display(32,46,P_LAT, P_OE,P_A,P_B,P_C);
// PxMATRIX display(64,32,P_LAT, P_OE,P_A,P_B,P_C,P_D);
//PxMATRIX display(64, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D, P_E);



// ISR for display refresh
void display_updater() {
  display.display(70);
}
void initializeWorld(){
  display.clearDisplay();
  delay(500);
  randomSeed(analogRead(5));
 
  for (byte i = 0; i < 48; i++) {
    for (byte j = 0; j < 32; j++) {
      WORLD[i][j] = random(0, 2);
    }
  }
}
void setup() {
  display.begin(8);
  display.setPanelsHeight(3);
  display.setPanelsWidth(1);
  display.clearDisplay();
  initializeWorld();
  display_ticker.attach(0.002, display_updater);
  yield(); 
}

void drawFrame() {
  display.clearDisplay();
  int imageHeight = 48;
  int imageWidth =32 ;
  for (int yy = 0; yy < imageHeight; yy++) {
    for (int xx = 0; xx < imageWidth; xx++) {
      if(WORLD[yy][xx] == 1) {
        display.drawPixel(xx, yy, display.color565(random(0,255), random(0,255), random(0,255)));
      } else {
        display.drawPixel(xx, yy, 0x0000);
      }
    }
  }
  delay(250);
}

// wrap val so that it lies inside [0, size-1],
//   allowing negative values down to -size
inline int8_t wrap(int8_t val, int8_t size)
{
  return (val + size) % size;
}

void loop() {
  if (step_GOL == 400) { // This if reboot the world after 60 generation to avoid static world
    step_GOL = 0;
    initializeWorld();
  }

  //This double "for" is used to update the world to the next generation
  //The buffer state is written on the EEPROM Memory
  for (byte i = 0; i < 48; i++) {
    for (byte j = 0; j < 32; j++) {

      byte num_alive = (WORLD[wrap(i - 1, 48)][wrap(j - 1, 32)] +
                        WORLD[wrap(i - 1, 48)][j] +
                        WORLD[wrap(i - 1, 48)][wrap(j + 1, 32)] +
                        WORLD[i][wrap(j - 1, 32)] +
                        WORLD[i][wrap(j + 1, 32)] +
                        WORLD[wrap(i + 1, 48)][wrap(j - 1, 32)] +
                        WORLD[wrap(i + 1, 48)][j] +
                        WORLD[wrap(i + 1, 48)][wrap(j + 1, 32)]);
      bool state = WORLD[i][j];
      
      //RULE#1 if you are surrounded by 3 cells --> you live
      if (num_alive == 3) {
        WORLD2[i][j] = 1;
      }
      //RULE#2 if you are surrounded by 2 cells --> you stay in your state
      else if (num_alive == 2) {
        WORLD2[i][j] = state;
      }
      //RULE#3 otherwise you die from overpopulation or subpopulation
      else {
        WORLD2[i][j] = 0;
      }
    }
  }

  //Updating the World
  for (byte i = 0; i < 48; i++) {
    for (byte j = 0; j < 32; j++) {
      WORLD[i][j] = WORLD2[i][j];
    }
  }

  //Displaying the world
  drawFrame();

  //Increasing the generation
  step_GOL++;
//  yield();
}
