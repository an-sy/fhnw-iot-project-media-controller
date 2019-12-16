#include <ESP8266WiFi.h>
#include <ArduinoHttpClient.h>
#include "secrets.h"

const int potiAnalogPin = A0;
int newVolume = 0;
int old_volume;
const int powerLedPin = 5;     
const int PowerButtonPin = 4;  
int powerState; 
void setup() {
  Serial.begin(115200);
  setPin();
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
}

void setPin() {
  pinMode(PowerButtonPin, INPUT);
  pinMode(potiAnalogPin, INPUT);
  pinMode(powerLedPin, OUTPUT);
  digitalWrite(powerLedPin, LOW);
  
  }

void loop() {
  int v = map(analogRead(potiAnalogPin), 0, 1023, 1000, 0);
  newVolume = v / 10;
  powerState = digitalRead(PowerButtonPin);

  // Change Volume if Value is +- 2 (prevent toggle) 
  if (newVolume >= (old_volume+2) || newVolume <= (old_volume-2)){
      changeVolume(newVolume);
   }
   delay(20);

  if (powerState == LOW) {
    digitalWrite(powerLedPin, HIGH);
    delay(200);
    } else {
          digitalWrite(powerLedPin, LOW);
      }
  }

void changeVolume(int volume) {
  String fullpath = path +String(volume);
  Serial.print("fullpath: ");
  Serial.println(fullpath);
  old_volume = volume;  
  WiFiClient wifi;
  HttpClient client = HttpClient(wifi, host, port);
  client.get(fullpath);
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);
  }
