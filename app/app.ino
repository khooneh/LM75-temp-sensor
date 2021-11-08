
#define VERSION "1.0"

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

void setup()
{
    Serial.begin(9600);
    display.setBrightness(0x0a);
    Serial.print(termo.getTemp());
  
}

void loop()
{   
    double tempC = termo.getTemp();
    int tempF = (tempC * 9 / 5) + 32;
    
    Serial.print(tempF);
    Serial.println(" oF");
   
    const uint8_t fahrenheit[] = {
      SEG_A | SEG_B | SEG_F | SEG_G,  // Circle
      SEG_A | SEG_F | SEG_E | SEG_G   // F
    };

    display.showNumberDec(tempF, false, 2, 0);
    display.setSegments(fahrenheit, 2, 2);
}
