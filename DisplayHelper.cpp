#include "DisplayHelper.h"

// Default constructor
DisplayHelper::DisplayHelper() { }

// Constructor
DisplayHelper::DisplayHelper(LiquidCrystal_I2C lcd)
{
    this->lcd = &lcd;
    this->lcd->init();
    this->lcd->backlight();
    this->lcd->setCursor(5,0);
    this->lcd->print("Smart");
    this->lcd->setCursor(5,1);
    this->lcd->print("Basket");
}

// Write text to display
void DisplayHelper::writeDisplay(char* msg)
{
    this->lcd->setCursor(0, 0);
    this->lcd->clear();
    this->lcd->print(msg);
}

void DisplayHelper::writeDisplay(char* msg, int* pos)
{
    this->lcd->setCursor(pos[0], pos[1]);
    this->lcd->print(msg);
}

// Clear display
void DisplayHelper::clearDisplay()
{
    this->lcd->setCursor(0,0);
    this->lcd->clear();
    this->lcd->setCursor(0, 2);
    this->lcd->clear();
}
