#include "displayUI.h"
#include <Arduino.h>
#include <TFT_eSPI.h>
//incldues sd fucntion file in order to read directory and files for display
#include "SD.h"

//sets up the tft object
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite cursor = TFT_eSprite(&tft);

//variables to store the positioning values of specific "tags"
//stores location information for the title of every screen
const int titleX = 10;
const int titleY = 5;
const int titleSize = 3;
//stores logic and information of cursor sprite
//creates separate current and default cursor values so that it can update live but also be reset properly on menu changes
bool isCursorOn = true;
int initialCursorX = 15;
int currentCursorX = initialCursorX;
int initialCursorY = 45;
int currentCursorY = initialCursorY;
unsigned long lastBlink = 0;
const long blinkInterval = 750;


//sets up the tft screen
void displaySetup()
{
    tft.begin();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);

    //more diagnostic code to check for details

        // setup_t tft_info;
    // tft.getSetup(tft_info);
    
    // Serial.println("\n--- TFT_eSPI Diagnostics Setup ---");
    // Serial.print("TFT_eSPI ver = "); Serial.println(tft_info.version);
    // Serial.print("Driver ID    = 0x"); Serial.println(tft_info.tft_driver, HEX);
    // Serial.print("Pixel width  = "); Serial.println(tft_info.tft_width);
    // Serial.print("Pixel height = "); Serial.println(tft_info.tft_height);
    // Serial.println("----------------------------------\n");

    //creates the cursor sprite
    cursor.createSprite(20,20);

    //draws the menu ui on start up to get the initial menu set up
    drawDisplayMenu();

}

void updateDisplay()
{
    tft.setCursor(titleX,titleY);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(titleSize);
    tft.println("Menu");
}

void drawDisplayMenu(int items)
{
    tft.drawFastHLine(0,35,240,TFT_WHITE);
    blinkCursor();
    Serial.println(cursor.getCursorX());
}

void blinkCursor()
{
    unsigned long currentMillis = millis();

    if (currentMillis - lastBlink >= blinkInterval)
    {
        lastBlink = millis();
        isCursorOn = !isCursorOn;

        cursor.fillSprite(TFT_BLACK);

        //if the cursor is now on that means it was previously off when the function ran
        if (isCursorOn)
        {
            cursor.fillTriangle(0,0, 20, 10, 0, 20, TFT_WHITE);
        }

        cursor.pushSprite(currentCursorX, currentCursorY);
    }

}
