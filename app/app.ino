
#define VERSION "1.0"

//---- Web Server ----//
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h> 

#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>   // Include the WebServer library
//

#include <inttypes.h>
#include <Wire.h>

#include <lm75.h>
TempI2C_LM75 termo = TempI2C_LM75(0x48,TempI2C_LM75::ten_bits);
// https://github.com/avishorp/TM1637


// Module connection pins (Digital Pins)
#include <TM1637Display.h>
#define CLK D6 
#define DIO D5

TM1637Display display(CLK, DIO);

// WiFi
ESP8266WiFiMulti wifiMulti;      
ESP8266WebServer server(80);     


#include <WiFiManager.h>         

WiFiManager wifiManager;

bool isCelsius = false;


// display change button 
#define DIS_PIN D8
int switchStatusLast = LOW;

void handleRoot();              
void handleNotFound();


void showF() {
    int temp = getTempF(); 

    const uint8_t fahrenheit[] = {
      SEG_A | SEG_B | SEG_F | SEG_G,  // Circle
      SEG_A | SEG_F | SEG_E | SEG_G   // F
    };

    display.showNumberDec(temp, false, 2, 0);
    display.setSegments(fahrenheit, 2, 2);

    isCelsius = false; 

    server.send(200, "text/plain", "Display should show Fahrenheit"); 

}

void showC() {
    int temp = termo.getTemp();
    const uint8_t celsius[] = {
      SEG_A | SEG_B | SEG_F | SEG_G,  // Circle
      SEG_A | SEG_F | SEG_E | SEG_D   // C
    };

    display.showNumberDec(temp, false, 2, 0);
    display.setSegments(celsius, 2, 2);

    isCelsius = true; 

    server.send(200, "text/plain", "Display should show Celsius"); 
}

int getTempF() {
    double tempC = termo.getTemp();
    int tempF = (tempC * 9 / 5) + 32;

    return tempF;
}

void sendF() {
  server.send(200, "text/plain", String(getTempF()));
}

void sendC() {
  server.send(200, "text/plain", String(termo.getTemp()));
}

void handleRoot() {
  server.send(200, "text/plain", String(getTempF())); 
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found");  
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(DIS_PIN, INPUT);

    digitalWrite(LED_BUILTIN, HIGH);

    Serial.begin(9600);
    display.setBrightness(0x0a);

    Serial.print("Temp ...\n");
    Serial.print(termo.getTemp());
    wifiManager.autoConnect("LM75-wifi-sensor");

    server.on("/", handleRoot);                
    // These will report the temp 
    server.on("/api/temp/f", sendF);                
    server.on("/api/temp/c", sendC);   


    // these changes the LED display
    server.on("/temp/f", showF);                
    server.on("/temp/c", showC);  

    server.onNotFound(handleNotFound); 

    server.begin();                           
    Serial.println("HTTP server started");

    if (WiFi.status() == WL_CONNECTED) {
      digitalWrite(LED_BUILTIN, LOW);
    } 
 
}


void loop() {   
    if (isCelsius) {
      showC();
    } else {
      showF();
    }

    int switchStatus = digitalRead(DIS_PIN);   

    if (switchStatusLast != switchStatus) {
      switchStatusLast = switchStatus;
      if (switchStatus == HIGH) {
        isCelsius = !isCelsius;
      }
    }

    // WiFi 
    server.handleClient();  
}
