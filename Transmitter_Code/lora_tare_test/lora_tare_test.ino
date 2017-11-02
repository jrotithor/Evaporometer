#include <EEPROM.h>
#include "HX711.h"

#define calibration_factor 430000//This value is obtained using the Calibration sketch (grams)

//load cell variables//
#define DOUT 5 //connecting the out and clock pins for the load cell
#define CLK 6

#include "HX711.h"
#define RESET_ADDRESS 0
#define soft_reset()        \
do                          \
{                           \
    wdt_enable(WDTO_15MS);  \
    for(;;)                 \
    {                       \
    }                       \
} while(0)
HX711 scale(DOUT, CLK);
const int buttonPin = SOME_NUMBER;   
void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));


void setup() {
  // put your setup code here, to run once:
  pinMode(buttonPin, INPUT);
  scale.set_scale(calibration_factor);
  if (EEPROM.read(RESET_ADDRESS)) {
    scale.tare();
    EEPROM.write(RESET_ADDRESS, 0);
  }
  attachInterrupt(digitalPinToInterrupt(buttonPin), pin_ISR, RISING);
  wdt_init();
}

void loop() {
  // put your main code here, to run repeatedly:
  //do something
}

void pin_ISR() {
  EEPROM.write(RESET_ADDRESS, 1);
  //http://www.atmel.com/webdoc/avrlibcreferencemanual/FAQ_1faq_softreset.html
  soft_reset();
  
}

void wdt_init(void)
{
    MCUSR = 0;
    wdt_disable();

    return;
}
