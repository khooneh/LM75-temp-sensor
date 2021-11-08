
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
ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80

void handleRoot();              // function prototypes for HTTP handlers
void handleNotFound();

String SSID = " "; 
String PW = " "; 

void setup()
{
    Serial.begin(9600);
    display.setBrightness(0x0a);

    Serial.println("Temp ...\n");
    Serial.print(termo.getTemp());
    wifiMulti.addAP(SSID.c_str(), PW.c_str());

    Serial.println("Connecting ...");
    int i = 0;
    while (wifiMulti.run() != WL_CONNECTED) { 
      delay(250);
      Serial.print(".");
    }
    Serial.print("Connected to ");
    Serial.println(WiFi.SSID());               
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP()); 

    if (MDNS.begin("esp8266")) {              // Start the mDNS responder for esp8266.local
      Serial.println("mDNS responder started");
    } else {
      Serial.println("Error setting up MDNS responder!");
    }

    server.on("/", handleRoot);                
    server.onNotFound(handleNotFound); 

    server.begin();                           
    Serial.println("HTTP server started");

}

void loop()
{   
    double tempC = termo.getTemp();
    int tempF = (tempC * 9 / 5) + 32;
    
//    Serial.print(tempF);
//    Serial.println(" oF");
   
    const uint8_t fahrenheit[] = {
      SEG_A | SEG_B | SEG_F | SEG_G,  // Circle
      SEG_A | SEG_F | SEG_E | SEG_G   // F
    };

    display.showNumberDec(tempF, false, 2, 0);
    display.setSegments(fahrenheit, 2, 2);

    // WiFi 
    server.handleClient();  
}

void handleRoot() {
  server.send(200, "text/plain", "Hello world!"); 
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found");  
}
