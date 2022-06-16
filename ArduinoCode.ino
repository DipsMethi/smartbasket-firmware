#include "DisplayHelper.h"
#include <SPI.h>
#include <MFRC522.h>
#include <GSMSim.h>
#include <SoftwareSerial.h>
#include <stdlib.h>

#define I2C_ADDR      0x27
#define NUM_ITEMS       6
#define SS_PIN          10
#define RST_PIN         9

#define D2 2
#define D3 3

int secRow[2] = {0, 1};
int amountFormat[2] = {7, 1};
int menu = 0;

byte count = 0;                                 
char itemCount;
int numItems = 6;
long code= 0;

int checkoutState, scrollState, selectState, abortState;    // Declare button state variables
int gPin = 5, wPin = 4, bPin = 7, rPin = 8;                 // Instantiate button pin numbers

bool isCardPresent = 0;
bool isLoggedIn = false;

DisplayHelper displayHelper;

// Instantiate program states
enum class PROGRAM_STATE {LOGIN, STANDBY, SCANNING, CHECKOUT, SEND_INVOICE, MENU, ABORT};
// Set current state
PROGRAM_STATE currentState = PROGRAM_STATE::SCANNING;
PROGRAM_STATE prevState = PROGRAM_STATE::SCANNING;

GSMSim gsm();
MFRC522 reader(SS_PIN, RST_PIN);

void setup()
{
  Serial.begin(115200);
  // Initialize RFID helper
  initRFID();
  // Initialize Wifi
  //initWifi();
  // Initialize gsm helper class
  initGSM();
  // Initialize buttons
  initButtons();
  // Instatiate display
  displayHelper = DisplayHelper( LiquidCrystal_I2C (I2C_ADDR, 16, 2) );
  delay(5 * 1000);
}

void loop()
{
  // Always check input buttons
  processButtons();
  // Check current state and execute handler
  switch (currentState)
  {
    case PROGRAM_STATE::LOGIN:
      LOGIN_HANDLER(); break;
    case PROGRAM_STATE::STANDBY:
      STANDBY_HANDLER(); break;
    case PROGRAM_STATE::SCANNING:
      SCANNING_HANDLER(); break;
    case PROGRAM_STATE::CHECKOUT:
      CHECKOUT_HANDLER(); break;
    case PROGRAM_STATE::SEND_INVOICE:
      //SEND_INVOICE_HANDLER(); break;
    case PROGRAM_STATE::MENU:
      MENU_HANDLER(); break;
    case PROGRAM_STATE::ABORT:
      ABORT_HANDLER; break;
  }
}

void initRFID()
{
    SPI.begin();
    reader.PCD_Init();
}

void initGSM()
{
  // Implement new GSMSim library 
}

void initButtons()
{     
    pinMode(gPin, INPUT_PULLUP);
    pinMode(wPin, INPUT_PULLUP);
    pinMode(bPin, INPUT_PULLUP);
    pinMode(rPin, INPUT_PULLUP);
}

void processButtons()
{
    checkoutState = digitalRead(gPin);
    scrollState = digitalRead(wPin);
    selectState = digitalRead(bPin);
    abortState = digitalRead(rPin);

    if (checkoutState == LOW)
    {
        displayHelper.writeDisplay("Checkout?");
        delay(2000);
  
        currentState = PROGRAM_STATE::MENU;
        prevState = PROGRAM_STATE::SCANNING;
    }
    else if (scrollState == LOW)
    {
        menu++;
        updateMenu();
        delay(100);
    }
    else if (selectState == LOW)
    {
        // Executes the action selected in the menu
        executeAction();
    }
    else if (abortState == LOW)
    {
        displayHelper.writeDisplay("Aborting...");
        delay(2000);
        currentState = PROGRAM_STATE::STANDBY;
    }    
    delay(100);
}

void LOGIN_HANDLER()
{
    if(currentState == PROGRAM_STATE::LOGIN)
    {
        displayHelper.writeDisplay("Please login");
        if( Serial.available() > 0)
        {
            String data = Serial.readString();
            Serial.print(data);
            delay(5000);
            if(data == "LoginSuccess")
                currentState = PROGRAM_STATE::SCANNING;
        }
    }
}

void STANDBY_HANDLER()
{
    displayHelper.writeDisplay("Standby");
}

void SCANNING_HANDLER()
{
    if ( currentState == PROGRAM_STATE::SCANNING)
    {  
        displayHelper.writeDisplay("Scanning items");
        if ( reader.PICC_IsNewCardPresent() )
        {
            if( reader.PICC_ReadCardSerial() )
            {   
                code = 0;           
                for (byte i = 0; i < reader.uid.size; i++)
                {
                    code = ((code+reader.uid.uidByte[i])*10);
                }
                Serial.print(code);
                displayHelper.writeDisplay("Item scanned");  
                delay(1000);       
            }
        }
    }
}

char* getCstr(String str)
{
    char* buf;
    str.toCharArray( buf, str.length() );
    return buf;
}

void CHECKOUT_HANDLER()
{
    if (prevState == PROGRAM_STATE::SCANNING && currentState == PROGRAM_STATE::CHECKOUT)
    {
        displayHelper.writeDisplay("Scan card");
        // Wait until bank card is scanned
        if ( reader.PICC_IsNewCardPresent() )
        {
                if( reader.PICC_ReadCardSerial() )
                {   
                    code = 0;           
                    for (byte i = 0; i < reader.uid.size; i++)
                    {
                        code = ((code+reader.uid.uidByte[i])*10);
                    }

                    Serial.print(code);
                    displayHelper.writeDisplay("Card scanned");  
                    delay(1000);
                    displayHelper.writeDisplay("Thank you");
                    delay(1000);
                    currentState = PROGRAM_STATE::STANDBY;       
                }
        }
    }
}

//void SEND_INVOICE_HANDLER()
//{
//    if (prevState == PROGRAM_STATE::CHECKOUT && currentState == PROGRAM_STATE::SEND_INVOICE)
//    {
//        // Send invoice
//        displayHelper.writeDisplay("Sending invoice");
//        delay(2000);
//    
//        char msg[message.length() + 1] = { };
//        strcpy(msg, (char*)message.c_str());
//    
//        if(gsm.smsSend("0795114017", msg))
//        {
//            displayHelper.writeDisplay("Invoice sent");
//            displayHelper.writeDisplay("Successfully!", secRow);
//            delay(5000);
//            displayHelper.writeDisplay("Thank You :)");
//            delay(5000);
//            currentState = PROGRAM_STATE::STANDBY;
//        }
//        else
//            displayHelper.writeDisplay("ERR03");
//    }
//}

void ABORT_HANDLER()
{
  // Clear item and price lists
//  memset(itemList, 0, sizeof(itemList));
//  memset(priceList, 0, sizeof(priceList));
  currentState = PROGRAM_STATE::STANDBY;
}

void MENU_HANDLER()
{
  updateMenu();
}

void updateMenu()
{
  switch (menu)
  {
    case 0:
      menu = 1;
      break;
    case 1:
      displayHelper.writeDisplay(">Yes");
      displayHelper.writeDisplay(" No", secRow);
      break;
    case 2:
      displayHelper.writeDisplay(" Yes");
      displayHelper.writeDisplay(">No", secRow);
      break;
    case 3:
      menu = 1;
      break;
  }
}

void executeAction()
{
  switch (menu)
  {
    case 1:
      action1();
      break;
    case 2:
      action2();
      break;
  }
}

// Checkout
void action1()
{
  currentState = PROGRAM_STATE::CHECKOUT;
  prevState = PROGRAM_STATE::SCANNING;
}

// Enter scanning state
void action2()
{
  currentState = PROGRAM_STATE::SCANNING;
  prevState = PROGRAM_STATE::SCANNING;
}
