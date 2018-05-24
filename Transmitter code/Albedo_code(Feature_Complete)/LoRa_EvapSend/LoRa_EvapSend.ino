/* Authors: Chet Udell, Marissa Kwon, Thomas DeBell
    Date: 06/26/2017
    THIS CODE IS IN COMPLETE AS OF 6/28/17 at 5:37 p.m do
  Description: Code for "Transmitter" LoRa radio Feather Board; initilizes
  LoRa radio transmitter and sends data to LoRa receiver on the same
  frequency.

  This code originated from Adafruit's website and has been
  adjusted for use in the Internet of Agriculture project.
  Beginning Researcher student Research at OPEnS Lab at Oregon State
  University.
  // Important Example code found at https://learn.adafruit.com/adafruit-rfm69hcw-and-rfm96-rfm95-rfm98-lora-packet-padio-breakouts/rfm9x-test

  APPENDIX *****************************
  SET-UP loop = 87, Void Loop(main) = 146, RTC Subroutines and loop= 241,
  Alarm Function = 277, 298,  Wakeup ISR= 311, InteruptPin set-up= 313,323
  ****************************************************8
*/
/* IMPORTANT NOTE:
 * To make the RTC library work with Feather M0, go to RTClibExtended.h
 * and add "#define _BV(bit) (1 << (bit))" to line 51 (without quote)
 * That should fix the _BV error
 */

// LoRa 9x_TX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client(transmitter)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example LoRa9x_RX

#include <SPI.h>
#include <RH_RF95.h> // Important Example code found at https://learn.adafruit.com/adafruit-rfm69hcw-and-rfm96-rfm95-rfm98-lora-packet-padio-breakouts/rfm9x-test
#include <RHReliableDatagram.h>
#include "HX711.h"  //https://learn.sparkfun.com/tutorials/load-cell-amplifier-hx711-breakout-hookup-guide
#include "LowPower.h" // from sparkfun low power library found here https://github.com/rocketscream/Low-Power
#include "RTClibExtended.h"// from sparkfun low power library found here https://github.com/FabioCuomo/FabioCuomo-DS3231/
//------------------------------------------------------------------------
// Humidity/Temperature Sensor Settings--------------------
//------------------------------------------------------------------------
//#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SHT31.h" // https://github.com/adafruit/Adafruit_SHT31
// ------------------------------------------------------------------------
// Infrared/Full Light Sensor Settings--------------------
//------------------------------------------------------------------------
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h> // https://github.com/adafruit/Adafruit_TSL2561
//------------------------------------------------------------------------
// Debug Mode, Set flag to 0 for normal operation
//------------------------------------------------------------------------
#define DEBUG 0
//------------------------------------------------------------------------
// LORA pins --------------------
//------------------------------------------------------------------------
//for feather m0
//#define RFM95_CS 8
//#define RFM95_RST 4
//#define RFM95_INT 3
// for 32u4
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7

#define SERVER_ADDRESS 2
//battery voltage read pin
#define VBATPIN A9

//super validator calibration variable//
#define calibration_factor 501000//This value is obtained using the Calibration sketch (grams)

//load cell variables//
#define DOUT 12 //connecting the out and clock pins for the load cell
#define CLK 13

//Taring pin
//#define TARE_PIN 10

//Global Set-up//
float temp, humidity, loadCell, albedo, measuredvbat;
String IDstring, tempString, humidityString, loadCellString, lightIRString, lightFullString, albedoString, vbatString, RTC_monthString, RTC_dayString, RTC_hrString, RTC_minString, RTC_timeString, stringTransmit;
int transmitBufLen; // length of transmit buffer
const int ID = 100;
uint16_t lightIR_1, lightFull_1, lightIR_2, lightFull_2;
//char transmitBuf[23]; // this needs to be the length of the transmission buffer

HX711 scale(DOUT, CLK);

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);
RHReliableDatagram manager(rf95, SERVER_ADDRESS);

// Evap code from Manuel instance of temp/humidity sensor
Adafruit_SHT31 sht31 = Adafruit_SHT31();

// Create instance of TSL2561 light sensors
Adafruit_TSL2561_Unified tsl_1 = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345); // top
Adafruit_TSL2561_Unified tsl_2 = Adafruit_TSL2561_Unified(TSL2561_ADDR_LOW, 11111); // bottom

// Create instance of DS3231 called RTC
RTC_DS3231 MyRTC; //we are using the DS3231 RTC

// declare/init RTC variables//
volatile bool TakeSampleFlag = false; // Flag is set with external Pin A0 Interrupt by RTC
volatile bool LEDState = false; // flag t toggle LED
volatile int HR = 8; // Hr of the day we want alarm to go off
volatile int MIN = 0; // Min of each hour we want alarm to go off
volatile int WakePeriodMin = 5;  // Period of time to take sample in Min, reset alarm based on this period (Bo - 5 min)
const byte wakeUpPin = 11;

////////////////////////
void setup()
{
  // convert ID into String, done up here because we only need this done once
  IDstring = String(ID, DEC);
  
  pinMode(wakeUpPin, INPUT_PULLUP);
  attachInterrupt(wakeUpPin, onRTCWake, FALLING);
  // Turns on temp and humid sensor //
  sht31.begin(0x44);
  //setting up tare pin
  //pinMode (TARE_PIN, INPUT_PULLUP);
  // load cell calibration
  scale.set_scale(calibration_factor); //This value is obtained by using the Calibration sketch
  scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0
  scale.power_down(); // Go into low power mode

  //LoRa transmission//
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  Serial.begin(9600);
  //while (!Serial); // waits for serial hardware to start up
#if DEBUG == 1
  //report all sensors present on system
  Serial.println(" LoRa Feather Transmitter Test!");
  Serial.println("HX711 scale");
  Serial.println("Starting Adafruit TSL2561 Test!");
#endif
  
  //check light sensors init
  while (!tsl_1.begin() && tsl_2.begin())
  {
  #if DEBUG == 1
    Serial.println("No sensor found ... check your wiring?");
  #endif  
    while (1);
  }
  
  #if DEBUG == 1
  Serial.println("Found the TSL2561 sensors");
  #endif
  /* Configure the sensor */
  configureSensor();
  //there's an option to print sensor details in example code - this requires addit'l functions and takes up too much space

  //manually reset LoRa
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  while (!manager.init()) {
    #if DEBUG == 1
      Serial.println("LoRa manager init failed"); //if print wiring may be wrong
    #endif
    while (1);
  }
  #if DEBUG == 1
    Serial.println("LoRa radio init OK!");
  #endif
  // checks if frequency is initialized
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    #if DEBUG == 1
      Serial.println("setFrequency failed");
    #endif
    while (1);
  }
  #if DEBUG == 1
    Serial.print("Set Freq to: ");
    Serial.println(RF95_FREQ);
  #endif

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips / symbol, CRC on
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);


  //RTC stuff init//
  InitalizeRTC();
  #if DEBUG == 1
    Serial.print("Alarm set to go off every "); Serial.print(WakePeriodMin); Serial.println("min from program time");
  #endif
  delay(1000);
}
////////////////////////// MAIN //////////////////////
void loop() {
  if (!DEBUG)
  {
    // Sleep the radio until needed
    rf95.sleep();
    // Enable SQW pin interrupt
    // enable interrupt for PCINT7...
    pciSetup(11);

    // Enter into Low Power mode here[RTC]:
    // Enter power down state with ADC and BOD module disabled.
    // Wake up when wake up pin is low.
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
    // <----  Wait in sleep here until pin interrupt
    // On Wakeup, proceed from here:
    //detachInterrupt(digitalPinToInterrupt(wakeUpPin));
    //clearAlarmFunction(); // Clear RTC Alarm
    //scale.power_up();
  }
  else
  {
    delay(15000); // period in DEBUG mode to wait between samples
    scale.power_up();
    TakeSampleFlag = 1;
  }
  if (TakeSampleFlag)
  {
    detachInterrupt(digitalPinToInterrupt(wakeUpPin)); 
    clearAlarmFunction(); // Clear RTC Alarm
    scale.power_up();
    // get RTC timestamp string
    DateTime now = MyRTC.now();
    uint8_t mo = now.month();
    uint8_t d = now.day();
    uint8_t h = now.hour();
    uint8_t mm = now.minute();

    RTC_monthString = String(mo, DEC);
    RTC_dayString = String(d, DEC);
    RTC_hrString = String(h, DEC);
    RTC_minString = String(mm, DEC);
    RTC_timeString = RTC_hrString + ":" + RTC_minString + "_" + RTC_monthString + "/" + RTC_dayString;
    //declaring a temporary array to hold values for the sensors
    float temp_ar[5];
    // Read sensors
  	for(int i = 0; i < 5; i++) {
  		temp_ar[i] = sht31.readTemperature();
  	}
  	temp = (temp_ar[0]+temp_ar[1]+temp_ar[2]+temp_ar[3]+temp_ar[4])/5; // degrees C
  	
  	for(int i = 0; i < 5; i++) {
  		temp_ar[i] = sht31.readHumidity();
  	}
  	humidity = (temp_ar[0]+temp_ar[1]+temp_ar[2]+temp_ar[3]+temp_ar[4])/5; // relative as a percent
  	
    for(int i = 0; i < 5; i++) {
		  temp_ar[i] = scale.get_units();
	  }
    loadCell = (temp_ar[0]+temp_ar[1]+temp_ar[2]+temp_ar[3]+temp_ar[4])/5;
		
    advancedRead();  // gets full and IR light values, save to global vars lightFull, lightIR

    measuredvbat = analogRead(VBATPIN); // reading battery voltage
    measuredvbat *= 2;    // we divided by 2, so multiply back
    measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
    measuredvbat /= 1024; // convert to voltage

    // RGB read - WIP

    // Manually power down the loadcell (wakes up when MCU wakes from sleep
    scale.power_down();
	
    if (isnan(temp)) {
      tempString = "nan";
    }
    else {
      tempString =  String(temp, 2); // 2 decimal places
    }
    if (isnan(humidity)) {
      humidityString = "nan";
      
    }
    else {
      humidityString =  String(humidity, 2); // same
    }
    // convert sensor data into string for concatenation
    //tempString =  String(temp, 2); // 2 decimal places
    //humidityString =  String(humidity, 2); // same
    loadCellString =  String(loadCell, 6); // 6 decimal places
    lightIRString = String(lightIR_1, DEC);
    lightFullString = String(lightFull_1, DEC);
    albedoString = String(albedo, 2);
    vbatString = String(measuredvbat, 1); //1 decimal place
    // RGB string - not yet possible with TSL2591

    //concatenate RGB and IR strings to stringTransmit
    stringTransmit = String(IDstring + "," + RTC_timeString + "," + tempString + "," + humidityString + "," + loadCellString + ","
                            + lightFullString + "," + lightIRString + "," + albedoString + "," + vbatString + "\0");//concates all strings into a big string
   
    // Calc len of transmit buffer:
    transmitBufLen = 1 + (char)stringTransmit.length(); // add 2 here to include last actual char

    // instantiate a transmit buffer of x len based on len of concat string above
    char transmitBuf[transmitBufLen];
    // converts long string of data into a character array to be transmitted
    stringTransmit.toCharArray(transmitBuf, transmitBufLen);
#if DEBUG == 1
    Serial.println(stringTransmit);
    Serial.println(transmitBufLen);
    Serial.println("Reading...");
    Serial.println(transmitBuf); // print to confirm transmit buff matches above string
    Serial.println("Sending to rf95_server");
#endif
    //begin sending to data to receiver (loops 3x)
    if(manager.sendtoWait((uint8_t*)transmitBuf, transmitBufLen, SERVER_ADDRESS)) {
#if DEBUG == 1
      Serial.println("Ok");
    }
    else {
      Serial.println("Send Failure");
#endif
    }

    // End big If statement from Sleep/Wake
    
    // Stuff that NEEDS to happen at the end
    // Reset alarm1 for next period
    setAlarmFunction();
    delay(75);  // delay so serial stuff has time to print out all the way
    TakeSampleFlag = false; // Clear Sample Flag
  }// endif takeSample Flag
} //endif Loop


//******************
// TSL2561 Subroutines
//******************
/**************************************************************************
    Configures the gain and integration time for the TSL2561
**************************************************************************/
void configureSensor(void)
{
  //for the first sensor:
  tsl_1.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  //tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  //tsl.enableAutoRange(true);          /* Auto-gain ... switches automatically between 1x and 16x */


  // Changing the integration time gives you a longer time over which to sense light
  // longer timelines are slower, but are good in very low light situtations!
  //tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  tsl_1.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

  
  //for the second sensor:
  tsl_2.setGain(TSL2561_GAIN_1X);
  tsl_2.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);
}
//**************************************************************************/
//    Show how to read IR and Full Spectrum at once and convert to lux
//**************************************************************************/
void advancedRead(void)
{
  //temporary array to hold the values 
  uint32_t lux1, lux2;
  uint16_t lightFull_ar[5], lightIR_ar[5];
  //sensor 1
  //Taking samples 5 times for average
  for (int i = 0; i < 5; i++) {
    tsl_1.getLuminosity(&lightFull_ar[i], &lightIR_ar[i]);
  }
  lightIR_1 = (lightIR_ar[0] + lightIR_ar[1] + lightIR_ar[2] + lightIR_ar[3] + lightIR_ar[4]) / 5;
  lightFull_1 = (lightFull_ar[0] + lightFull_ar[1] + lightFull_ar[2] + lightFull_ar[3] + lightFull_ar[4]) / 5;
  //sensor 2
    //Taking samples 5 times for average
  for (int i = 0; i < 5; i++) {
    tsl_2.getLuminosity(&lightFull_ar[i], &lightIR_ar[i]);
  }
  lightIR_2 = (lightIR_ar[0] + lightIR_ar[1] + lightIR_ar[2] + lightIR_ar[3] + lightIR_ar[4]) / 5;
  lightFull_2 = (lightFull_ar[0] + lightFull_ar[1] + lightFull_ar[2] + lightFull_ar[3] + lightFull_ar[4]) / 5;
  
  //measuring albedo
  lux1 = tsl_1.calculateLux(lightFull_1, lightIR_1);
  lux2 = tsl_2.calculateLux(lightFull_2, lightIR_2);
  albedo = lux1/lux2;
}


//******************
// RTC Subroutines
//******************
void InitalizeRTC()
{
  // RTC Timer settings here
  if (! MyRTC.begin()) {
#if DEBUG == 1
    Serial.println("Couldn't find RTC");
#endif
    while (1);
  }
  // This may end up causing a problem in practice - what if RTC looses power in field? Shouldn't happen with coin cell batt backup
  if (MyRTC.lostPower()) {
#if DEBUG == 1
    Serial.println("RTC lost power, lets set the time!");
#endif
    // following line sets the RTC to the date & time this sketch was compiled
    MyRTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  //clear any pending alarms
  clearAlarmFunction();

  // Querry Time and print
  DateTime now = MyRTC.now();
#if DEBUG == 1
  Serial.print("RTC Time is: ");
  Serial.print(now.hour(), DEC); Serial.print(':'); Serial.print(now.minute(), DEC); Serial.print(':'); Serial.print(now.second(), DEC); Serial.println();
#endif
  //Set SQW pin to OFF (in my case it was set by default to 1Hz)
  //The output of the DS3231 INT pin is connected to this pin
  //It must be connected to arduino Interrupt pin for wake-up
  MyRTC.writeSqwPinMode(DS3231_OFF);

  //Set alarm1
  setAlarmFunction();
}

// *********
// RTC helper function
// Function to query current RTC time and add the period to set next alarm cycle
// *********
void setAlarmFunction()
{
  DateTime now = MyRTC.now(); // Check the current time

  // Calculate new time
  MIN = (now.minute() + WakePeriodMin) % 60; // wrap-around using modulo every 60 sec
  HR  = (now.hour() + ((now.minute() + WakePeriodMin) / 60)) % 24; // quotient of now.min+periodMin added to now.hr, wraparound every 24hrs
#if DEBUG == 1
  Serial.print("Resetting Alarm 1 for: "); Serial.print(HR); Serial.print(":"); Serial.println(MIN);
#endif

  //Set alarm1
  MyRTC.setAlarm(ALM1_MATCH_HOURS, MIN, HR, 0);   //set your wake-up time here
  MyRTC.alarmInterrupt(1, true);

} 

//*********
// RTC helper function
// When exiting the sleep mode we clear the alarm
//*********
void clearAlarmFunction()
{
  //clear any pending alarms
  MyRTC.armAlarm(1, false);
  MyRTC.clearAlarm(1);
  MyRTC.alarmInterrupt(1, false);
  MyRTC.armAlarm(2, false);
  MyRTC.clearAlarm(2);
  MyRTC.alarmInterrupt(2, false);
}
//**********************
// RTC Interrupt Function
//********************
void onRTCWake() {
  TakeSampleFlag = true;
  //Serial.println("Interrupt called!");
}

//**********************
// Wakeup in SQW ISR
//********************
// Function to init PCI interrupt pin
// Pulled from: https://playground.arduino.cc/Main/PinChangeInterrupt

void pciSetup(byte pin)
{
  *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
  PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
  PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}


// Use one Routine to handle each group

ISR (PCINT0_vect) // handle pin change interrupt for D8 to D13 here
{
  if (digitalRead(11) == LOW)
    TakeSampleFlag = true;
}




//**********************
// Tare Interrupt Function
//********************
/*
void onTareCall() {
  scale.tare();
  long offset = scale.get_offset();
  
}
*/
  
