
/*
 * Portions of this code are adapted from Aurora: https://github.com/pixelmatix/aurora
 * Copyright (c) 2014 Jason Coon
 *
 * Portions of this code are adapted from LedEffects Plasma by Robert Atkins: https://bitbucket.org/ratkins/ledeffects/src/26ed3c51912af6fac5f1304629c7b4ab7ac8ca4b/Plasma.cpp?at=default
 * Copyright (c) 2013 Robert Atkins
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
 
//#define USE_CUSTOM_PINS // uncomment to use custom pins, then provide below

#define A_PIN  26
#define B_PIN  4
#define C_PIN  27
#define D_PIN  2
#define E_PIN  21 

#define R1_PIN   5
#define R2_PIN  19
#define G1_PIN  17
#define G2_PIN  16
#define B1_PIN  18
#define B2_PIN  25

#define CLK_PIN  14
#define LAT_PIN  15
#define OE_PIN  13
 
 
#include <ESP32-RGB64x32MatrixPanel-I2S-DMA.h>
RGB64x32MatrixPanel_I2S_DMA dma_display;
#include <Ticker.h>
#include <FastLED.h>

#include "virtualGFX.h"

Ticker display_ticker;

int time_counter = 0;
int cycles = 0;




#define NUM_ROWS 2 // Number of rows panels in your overall display
#define NUM_COLS 2 // number of panels in each row

#define PANEL_RES_X 32 // Number of pixels wide of each indivdual panel e.g. pixel purse has 32 
#define PANEL_RES_Y 16 // Number of pixels tall of each panel e.g. pixel purse has 16

bool WORLD[16][32]; // Creation of the world
bool WORLD2[16][32]; // Creation of the world
int step_GOL; //used to know the generation

int vResX = NUM_COLS * PANEL_RES_X;
int vResY = NUM_ROWS * PANEL_RES_Y;
VirtualGFX virtualDisp(dma_display, NUM_ROWS, NUM_COLS, PANEL_RES_X, PANEL_RES_Y, vResX, vResY);


void initializeWorld(){
  dma_display.clearScreen();
  delay(500);
  randomSeed(analogRead(5));
 
  for (byte i = 0; i < 16; i++) {
    for (byte j = 0; j < 32; j++) {
      WORLD[i][j] = random(0, 2);
    }
  }
}

void setup() {
  
  Serial.begin(115200);
  
  initializeWorld();



#ifdef USE_CUSTOM_PINS
  dma_display.begin(R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN );  // setup the LED matrix
#else
  dma_display.begin();
#endif
 
  // fill the screen with 'black'
  //dma_display.fillScreen(dma_display.color444(0, 0, 0));
  virtualDisp.fillScreen(dma_display.color444(0, 0, 0));

  // Set current FastLED palette
  yield();

}

void drawFrame() {
  dma_display.clearScreen(); // Must fill the DMA buffer with the initial output bit sequence or the panel will display garbage
  dma_display.flipDMABuffer(); // flip to backbuffer 1
  dma_display.clearScreen(); // Must fill the DMA buffer with the initial output bit sequence or the panel will display garbage
  dma_display.flipDMABuffer(); // backbuffer 0

  int imageHeight = 16;
  int imageWidth = 32;
  for (int yy = 0; yy < imageHeight; yy++) {
    for (int xx = 0; xx < imageWidth; xx++) {
      if(WORLD[yy][xx] == 1) {
        dma_display.drawPixel(xx, yy, dma_display.color565(255, 100, 100));
      } else {
        dma_display.drawPixel(xx, yy, 0x0000);
      }
    }
  }
  delay(350);
}

void loop() {
  
   for (int x = 0; x <  virtualDisp.width(); x++) {
  if (step_GOL == 120) { // This if reboot the world after 60 generation to avoid static world
    step_GOL = 0;
    initializeWorld();
  }
  //This double "for" is used to update the world to the next generation
  //The buffer state is written on the EEPROM Memory

  for (byte i = 0; i < 16; i++) {
    for (byte j = 0; j < 32; j++) {

      if (i == 0 || i == 15 || j == 0 || j == 31) // I choose to keep the border at 0
      {
        WORLD2[i][j] = 0;
      }
      else {
        byte num_alive = WORLD[i - 1][j - 1] + WORLD[i - 1][j] + WORLD[i - 1][j + 1] + WORLD[i][j - 1] + WORLD[i][j + 1] + WORLD[i + 1][j - 1] + WORLD[i + 1][j] + WORLD[i + 1][j + 1];
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
  }

  //Updating the World
  for (byte i = 0; i < 16; i++) {
    for (byte j = 0; j < 32; j++) {
      WORLD[i][j] = WORLD2[i][j];
    }
  }

  //Displaying the world
  drawFrame();

  //Increasing the generation
  step_GOL++;
//  yield();
        
} // end loop
}
