
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "BluefruitConfig.h"

#if SOFTWARE_SERIAL_AVAILABLE
  #include <SoftwareSerial.h>
#endif

#define BLUEFRUIT_SPI_CS            8
#define FACTORYRESET_ENABLE         1
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "MODE"

byte rSeed;
byte boolVow = 0x00;
byte errorResult;
byte strikes = 0x00;
byte Level = 0x00;

Adafruit_SSD1306 display(128, 32, &Wire, -1);

int i = 0;
char alphabet[26] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
bool First = true;
byte totalMin = 0x00;

byte simon = 0x38;
byte password = 0x29;
byte clck = 0x25;
byte devList[] = {simon, password, clck};
bool finList[] = {false, false, false};

byte state; 

bool game_over = false;

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

byte get_level() {
  byte a = 0xFF;
  byte difLevel;
  
  while(ble.available() && (a == 0xFF)) {

    a = ble.read(); // getting data from Bluetooth module as an integer
    difLevel = (0x30 ^ a); // ASCII characters for integers start at 0d48 = 0x30
    Serial.print(a);
    }
    return difLevel;
}

byte get_tot_mins(byte difLevel) {
  byte totalMins;
  
   if (0x0 < difLevel <= 0x3) {
      totalMins = 0x5;
    }
    else if (0x3 < difLevel <= 0x6) {
      totalMins = 0x6;
    }
    else {
      totalMins = 0x7;
    }

    return totalMins;
}

void send_data(byte peripheral, byte data) {
  while(1)
    {
       Serial.print("Here\n");
       Wire.beginTransmission(peripheral);
       Wire.write(data);
       errorResult = Wire.endTransmission();                 // if a device is present, it will send an ack and "0" will be returned from function
       delay(1000);
  
      if (errorResult == 0)                                 // "0" means a device at current address has acknowledged the serial communication
      {
        Serial.print("Transmission Successful\n");
        break;
      }
    }
}

/* data will come as a 0, 1, 2 */
byte get_data(byte peripheral) {
  byte data = 0x0;

  Wire.requestFrom(peripheral, 1);

  while (Wire.available()) {
    data = Wire.read();
  }
  return data;
}

byte init_display() {
  byte contVow;
  int i;
  display.clearDisplay();
    display.setCursor(1,1);
    display.setTextSize(1.5);
    display.setTextColor(WHITE);
    
    randomSeed(rSeed);
  
    display.print("Serial Number:");
    display.display();
  
    display.setCursor(1,16);
    display.setTextSize(2);
    
    i = random(26);
    display.print(alphabet[i]);
    if ((i == 0) or (i == 4) or (i == 8) or (i == 14) or (i == 20))
    {
      contVow = 0x01;
    }
    display.display();
    i = random(26);
    display.print(alphabet[i]);
    if ((i == 0) or (i == 4) or (i == 8) or (i == 14) or (i == 20))
    {
      contVow = 0x01;
    }
    display.display();
    i = random(10);
    display.print(i);
    display.display();
    i = random(26);
    display.print(alphabet[i]);
    if ((i == 0) or (i == 4) or (i == 8) or (i == 14) or (i == 20))
    {
      contVow = 0x01;
    }
    display.display();
    i = random(26);
    display.print(alphabet[i]);
    if ((i == 0) or (i == 4) or (i == 8) or (i == 14) or (i == 20))
    {
      contVow = 0x01;
    }
    display.display();
    i = random(10);
    display.print(i);
    display.display();
    delay(1000);
    return contVow;
}

void setup() {
  // put your setup code here, to run once:
while (!Serial);  // required for Flora & Micro

  delay(500);

  Serial.begin(115200);

  Serial.print("BEGINNING OF SETUP \n");
/*  Serial.println(F("Adafruit Bluefruit Command <-> Data Mode Example"));
  Serial.println(F("------------------------------------------------"));

  // Initialise the module 
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    // Perform a factory reset to make sure everything is in a known state 
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }

  // Disable command echo from Bluefruit 
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  // Print Bluefruit information 
  ble.info();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in UART mode"));
  Serial.println(F("Then Enter characters to send to Bluefruit"));
  Serial.println();

  ble.verbose(false);  // debug info is a little annoying after this point!

  // Wait for connection 
  while (! ble.isConnected()) {
      delay(500);
  }

  Serial.println(F("******************************"));

  // LED Activity command is only supported from 0.6.6
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    // Change Mode LED Activity
    Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }

  // Set module to DATA mode
  Serial.println( F("Switching to DATA mode!") );
  ble.setMode(BLUEFRUIT_MODE_DATA);

  Serial.println(F("******************************"));*/
  Serial.print("AFTER COMMENTS\n");

  rSeed = analogRead(A3)*micros();
  Serial.print("AFTER RANDOM");
  Wire.begin();
  Serial.println("AFTER I2C");
  pinMode(A0, OUTPUT); // Strike 1: Left LED
  pinMode(A1, OUTPUT); // Strike 2: Middle LED
  pinMode(A2, OUTPUT); // Strike 3: Right LED
  pinMode(10, INPUT); // Switch to start program
  digitalWrite(A0, LOW);
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  Serial.println("BEFORE OLED BEGIN");
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  Serial.println("AFTER OLED BEGIN");
  display.clearDisplay();
  Serial.print("END OF SET UP");
}

void loop() {
  // put your main code here, to run repeatedly:

  while (!digitalRead(10));
  
  Serial.print("BEGINNING OF LOOP\n");
  
  Level = 0x1;

  totalMin = get_tot_mins(Level);

  boolVow = init_display();


// sending data to simon says 
  send_data(simon, Level);
  send_data(simon, boolVow);
  send_data(simon, rSeed); 

  /* sending data to password game */

  send_data(password, Level);
  send_data(password, rSeed);
  
  // sending data to clock module 

  send_data(clck, totalMin); 

  /* constant polling till strikes = 3, complete all modules, or time is up */

  while ((strikes < 3) && !game_over) {
    Serial.println("here");
    state = get_data(devList[i%3]);

    switch (state) {

      case 0x0: break;

      case 0x1:
        strikes = strikes + 0x1;
        if (devList[i%3] != simon) {
          send_data(simon, strikes);
        } 
        break;

      case 0x2:
        finList[i%3] = true;
        break;
        
    }
    
    if (strikes == 1){
      digitalWrite(A0, HIGH);
    }

    else if (strikes == 2){
      digitalWrite(A1, HIGH);
    }
    
    else if (strikes == 3){
      digitalWrite(A2, HIGH);
      }

    i++;

    /* determine game over */
    if (((finList[0] & finList[1]) == true) || (finList[2] == true)) {
      game_over = true;
    }
  }

  /* determining won or lost */ 

}
