#include "displayUI.h"
#include <Arduino.h>
#include <TFT_eSPI.h>
//incldues sd fucntion file in order to read directory and files for display
#include "SD.h"

//sets up the tft object
TFT_eSPI tft = TFT_eSPI();
//cursor sprite
TFT_eSprite cursor = TFT_eSprite(&tft);
//selected item highlight sprite
TFT_eSprite highlight = TFT_eSprite(&tft);

//sprite just for clearing the old items off the screen
TFT_eSprite itemText = TFT_eSprite(&tft);

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

//vars to store the location information for the item select highlight
//the x value should never change and then y value is just the same as the cursor Y value
const int itemHighlightX = 40;

//variable to store the default location of the first item in a folder
int itemTextSize = 2; 
int initialItemX = 45;
//the x value should always be the same but well add it anyway
int currentItemX = initialItemX;
//y is 45 + a small spacing buffer
//since size 2 text is 16 px tall we need a 2px buffer above and below
int initialItemY = 48;
int currentItemY = initialItemY;
int itemSpacingY = 25;

//stores the current item the cursor should be pointing at
int selectedItem = 0;
//needed to create separate song selected and current row vars to handle item lists longer than 11 (the max items that can be displayed at once)
int selectedRow = 0;

//creates String vector to store the items of the current directory
std::vector<String> songs;

//var to store the length of the dynamically changed item highlight rect
int maxItemHighlightWidth = 200;
int currentItemHighlightWidth = 200;

//stores the current screen that the display is on
String currentScreen;
int firstVisibleItem = 0;
int previousFirstVisibleItem = 0;

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
    
    //turns text wrapping off
    tft.setTextWrap(false, false);

    //creates the cursor sprite
    cursor.createSprite(20,20);
    //creates highlight sprite
    highlight.createSprite(200,24);
    itemText.createSprite(200,24);

    //draws the menu ui on start up to get the initial menu set up

}

void updateDisplay(int direction)
{
    //changes the selected item to point to the correct new value
    selectedItem += direction;
    Serial.println(selectedItem);
    //checks to make sure that the desired scroll direction is even possible
    //if user tries to scroll too far up it will just wrap around to the bottom
    if (selectedItem < 0)
    {
        //sets selected item to the last song in the list 
        selectedItem = songs.size() - 1;
    }
    //if the user tries to scroll past the last song itll automatically wrap around to the top
    else if (selectedItem >= songs.size())
    {
        selectedItem = 0;
    }
    selectedRow = selectedItem;



    //if it scrolls past the max items that can be shown but not yet the total items on the list, then set to 10 so that the next item down can be shown
    if (selectedRow > 10)
    {
        selectedRow = 10;
    }
    firstVisibleItem = selectedItem - selectedRow;
    Serial.println(selectedItem);
    Serial.println(selectedRow);

    if (firstVisibleItem != previousFirstVisibleItem)
    {
        Serial.println("drawing screen again");
        drawScreen(currentScreen, songs);
        previousFirstVisibleItem = firstVisibleItem;
    }

    //first clears the current sprite location
    cursor.fillSprite(TFT_BLACK);
    cursor.pushSprite(currentCursorX, currentCursorY);

    tft.drawRect(itemHighlightX, currentCursorY-2, currentItemHighlightWidth ,24, TFT_BLACK);

    //checks the length of the selected item and sets the rect width to match it or be a default of 200
    if (tft.textWidth(songs[selectedItem]) >= 200)
    {
        currentItemHighlightWidth = maxItemHighlightWidth;
    }
    else
    {
        currentItemHighlightWidth = tft.textWidth(songs[selectedItem]) + 10; //adds a small pixel buffer
    }
    
    //then changes the cursor y value to point to the new direction
    //CHANGE: currentCursorY is not longer updated incrementally but rather based on the selected item
    currentCursorY = initialCursorY + (itemSpacingY * selectedRow);

    
    //immediatly draws in the cursor and highlight so that there isnt prologned dark periods
    cursor.fillTriangle(0,2, 18, 10, 0, 18, TFT_WHITE);
    cursor.pushSprite(currentCursorX,currentCursorY);

    //needs to first reset the entire sprite to black to clear any different length highlight boxes
    highlight.fillSprite(TFT_BLACK);
    highlight.drawRect(0, 0, currentItemHighlightWidth, 24, TFT_WHITE);
    highlight.pushSprite(itemHighlightX, currentCursorY-2, TFT_BLACK);
}

void drawScreen(String title, const std::vector<String>& items)
{
    //stores the directory items in a global variable
    songs = items;
    //and the directory name
    currentScreen = title; 
    //draws the initial menu display
    tft.setCursor(titleX,titleY);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(titleSize);
    tft.println(title);
    tft.drawFastHLine(0,35,240,TFT_WHITE);

    //sets different text size for the songs 
    tft.setTextSize(itemTextSize);
 

    for (int i = (selectedItem - selectedRow); i <= (selectedItem - selectedRow) + 10; i++)
    {   
        //fills the sprite to black to erease all the text underneath
        itemText.fillSprite(TFT_BLACK);
        //first sets the cursory value
        currentItemY = initialItemY + (itemSpacingY * (i - selectedItem + selectedRow)); 
        //clears the item slot before writting the new item name
        itemText.pushSprite(currentItemX, currentItemY);
        //sets the cursor to the first item "slot"
        tft.setCursor(currentItemX, currentItemY);
        //prints the item name
        tft.print(songs[i]);
    }

}

void blinkCursor()
{
    unsigned long currentMillis = millis();

    if (currentMillis - lastBlink >= blinkInterval)
    {
        lastBlink = millis();
        isCursorOn = !isCursorOn;

        //initially sets both sprites to disappear
        cursor.fillSprite(TFT_BLACK);
        
        //since the rectangle sprite has trouble erasing itself without the filter to protect the text behind it i just decided to draw over it normaly
        tft.drawRect(itemHighlightX, currentCursorY-2, currentItemHighlightWidth ,24, TFT_BLACK);

        //if the cursor is now on that means it was previously off when the function ran
        if (isCursorOn)
        {
            cursor.fillTriangle(0,2, 18, 10, 0, 18, TFT_WHITE);
            //needs to first reset the entire sprite to black to clear any different length highlight boxes
            highlight.fillSprite(TFT_BLACK);
            highlight.drawRect(0,0,currentItemHighlightWidth ,24,TFT_WHITE);
            highlight.pushSprite(itemHighlightX, currentCursorY-2, TFT_BLACK);

        }

        cursor.pushSprite(currentCursorX, currentCursorY);
    }

}
