#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#ifndef DispalyHelper_h
#define DisplayHelper_h

#define COL   20
#define ROW    4

class DisplayHelper
{
    public:
      DisplayHelper();
      DisplayHelper(LiquidCrystal_I2C);
      void writeDisplay(char*);
      void writeDisplay(char*, int*);
      void clearDisplay();

      LiquidCrystal_I2C *lcd;  
};
#endif
