#include <ESP8266WiFi.h>
#include <ArduinoHttpClient.h>
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include "secrets.h"
#include "config.h"

const uint16_t irTransmitterPin = 2; 
const int potiAnalogPin = A0;
const int powerLedPin = 5;     
const int PowerButtonPin = 4; 
const int progUP =15;
const int progDown = 16; 
int newVolume = 0;
int old_volume;
bool state1 = false;
bool state2 = false;

IRsend irsend(irTransmitterPin);

// Start initial settings
void setup() {
  Serial.begin(115200);
  setPin();
  irSetup();
  Serial.print("\nConnecting to network ");
  Serial.println(SECRET_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(100); // keeps watchdog happy
  }
  Serial.print("Connected to network, local IP = ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());

   // Group Sonos Speakers
   WiFiClient wifi;
   HttpClient client = HttpClient(wifi, sonosHost, sonosPort);
   client.get(sonosPathJoin);
   
}

void irSetup() {
    irsend.begin();
  #if defined(ESP8266)
    Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
  #else  // ESP8266
    Serial.begin(115200, SERIAL_8N1);
  #endif  // ESP8266
  }

void setPin() {
  pinMode(PowerButtonPin, INPUT);
  pinMode(progUP, INPUT);
  pinMode(progDown, INPUT);
  pinMode(potiAnalogPin, INPUT);
  pinMode(powerLedPin, OUTPUT);
  digitalWrite(powerLedPin, LOW);
  }
// End initial settings


// Start loop
void loop() {
    
  int v = map(analogRead(potiAnalogPin), 0, 1023, 1000, 0);
  newVolume = v / 10;
  // Change Volume if Value is +- 2 (prevent toggle) 
  if (newVolume >= (old_volume+2) || newVolume <= (old_volume-2)){
      changePlaybarVolume(newVolume);
   }
   delay(20);

  // SwisscomTv on / off
  if (state1 == false){
    swisscomTvOn();
  } 
  
  if (state2 == true){
    swisscomTvOff();
  }

  // Programm up/down
  if (digitalRead(progUP) == HIGH) {
        Serial.println("prog UP");
    irsend.sendPronto(SwisscomProgUp, 26);
    digitalWrite(powerLedPin, HIGH);
    delay(200);
    } else {
      digitalWrite(powerLedPin, LOW);
      }
  if (digitalRead(progDown) == HIGH) {
    Serial.println("prog Down");
  irsend.sendPronto(SwisscomProgDown, 26);
  digitalWrite(powerLedPin, HIGH);
  delay(200);
  } else {
    digitalWrite(powerLedPin, LOW);
    }
  }

// End loop


// Swisscom-TV ON 
void swisscomTvOn(){
  if (digitalRead(PowerButtonPin) == LOW) {
    Serial.println("Power");
    irsend.sendPronto(SwisscomPower, 28);
    state1 = true;
    state2 = true;
    digitalWrite(powerLedPin, HIGH);
    delay(200);
    menuOff();
    } else {
      digitalWrite(powerLedPin, LOW);
      }     
}

// Swisscom-TV OFF
void swisscomTvOff(){
      if (digitalRead(PowerButtonPin) == LOW) {
    Serial.println("Power");
    irsend.sendPronto(SwisscomPower, 28);
    state1 = false;
    state2 = false;
    digitalWrite(powerLedPin, HIGH);
    delay(200);
    } else {
      digitalWrite(powerLedPin, LOW);
      } 
}

// Swisscom-TV Menu OFF
void menuOff(){
  delay(2000);
  irsend.sendPronto(SwisscomMenu, 26);
}

// control Playbar-Sonosspeaker volume
void changePlaybarVolume(int volume) {
  // offset volume playbar volume
  int playbarVol = volume-30;
  String fullpath1 = sonosPath1 +String(playbarVol);
  old_volume = volume;  
  WiFiClient wifi;
  HttpClient client = HttpClient(wifi, sonosHost, sonosPort);
  client.get(fullpath1);
  changeWohnzimmerVolume(volume);
  }

// control Wohnzimmer-Sonosspeaker volume
void changeWohnzimmerVolume(int volume){
  String fullpath2 = sonosPath2 +String(volume); //
  WiFiClient wifi;
  HttpClient client = HttpClient(wifi, sonosHost, sonosPort);
  client.get(fullpath2);
  delay(500); 
}
