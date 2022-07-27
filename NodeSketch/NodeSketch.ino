#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>

#include <stdio.h>
#include <string.h>
#include <algorithm>

#define D4 4
#define D5 5

const char *URL                     = "http://hostname/SmartBasket/WebAPI/cart.php";

//const char *_SSID                    = "";       // Wifi Network Name
//const char *_PASSWORD                = "";       // Wifi Network Key

String UID = "";

// Instantiate program states
enum class PROGRAM_STATE {LOGIN, STANDBY, SCANNING, CHECKOUT, SEND_INVOICE, MENU, ABORT};

// Set current state
PROGRAM_STATE currentState = PROGRAM_STATE::SCANNING;
PROGRAM_STATE prevState = PROGRAM_STATE::SCANNING;

void setup()
{
  Serial.begin(115200);
  //serial.begin(115200);
  //while(!serial) { ; }
  pinMode(LED_BUILTIN, OUTPUT);

  //WiFi.mode(WIFI_STA);
  WiFi.begin(_SSID, _PASSWORD);
  //Serial.print("IP Addr: ");
  while (WiFi.status() != WL_CONNECTED)
    BlinkLED(2);
}

void loop()
{
    //Serial.println(WiFi.localIP());
    WIFI_CONNECTION_STATUS();
    // Check current state and execute handler
    switch (currentState)
    {
      case PROGRAM_STATE::LOGIN:
        LOGIN_HANDLER(); break;
      case PROGRAM_STATE::STANDBY:
        //STANDBY_HANDLER(); 
        break;
      case PROGRAM_STATE::SCANNING:
        SCANNING_HANDLER(); break;
      case PROGRAM_STATE::CHECKOUT:
        CHECKOUT_HANDLER(); break;
      case PROGRAM_STATE::SEND_INVOICE:
        //SEND_INVOICE_HANDLER(); 
        break;
      case PROGRAM_STATE::MENU:
        //MENU_HANDLER(); 
        break;
      case PROGRAM_STATE::ABORT:
        //ABORT_HANDLER;
        break;
   }   
}

void LOGIN_HANDLER()
{
    if(currentState == PROGRAM_STATE::LOGIN)
    {
        if(WiFi.status() == WL_CONNECTED)
        {
            HTTPClient http;
            String postData = "method=GET_USER";
            http.begin("http://192.168.27.217/SmartBasket/WebAPI/user.php");
            http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        
            int httpCode = http.POST(postData);
            String payload = http.getString();
            //Serial.println(postData);
            //Serial.println(payload);
            UID = payload;
            
            if(payload != "No data")
            {
                Serial.print("LoginSuccess");
                currentState = PROGRAM_STATE::SCANNING;
                delay(1000);
            }
        }
    }
}

void SCANNING_HANDLER()
{
    if(currentState == PROGRAM_STATE::SCANNING)
    {
        if( Serial.available() )
        {
            String dat = Serial.readString();
            if(dat == "2624570")
            {
                currentState = PROGRAM_STATE::CHECKOUT;
                prevState = PROGRAM_STATE::SCANNING;
            }
            else
                sendCardID(dat);
        }
    }
}

void CHECKOUT_HANDLER()
{
                HTTPClient http;
                String postData = "method=SCAN_CARD";
                http.begin("http://192.168.1.103/SmartBasket/WebAPI/cart.php");
                http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        
                int httpCode = http.POST(postData);
                String payload = http.getString();
}

void WIFI_CONNECTION_STATUS()
{
    switch( WiFi.status() )
    {
        case WL_CONNECTED: BlinkLED(0.5); break;
        case WL_DISCONNECTED: BlinkLED(2); break;
    }
}

void sendCardID(String id)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        String postData = "method=INSERT_ITEM&cardid=" + id;
        http.begin(URL);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
        int httpCode = http.POST(postData);
        String payload = http.getString();
        //Serial.println(postData);
        Serial.println(payload);
    
        http.end();
    }
}

void BlinkLED(float sec)
{
  digitalWrite(LED_BUILTIN, LOW);
  delay(sec * 1000);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(sec * 1000);
}

String split(char str[], int index)
{
    char* pch = strtok(str, "=");
}

String getTotal()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        String postData = "method=GET_TOTAL";
        http.begin(URL);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
        int httpCode = http.POST(postData);
        String payload = http.getString();
        //Serial.println(postData);
        Serial.println(payload);
    
        http.end();
    }
}
