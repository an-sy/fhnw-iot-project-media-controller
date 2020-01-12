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
const int progUP = 15;
const int progDown = 16;
int newVolume = 0;
int old_volume;
bool powerState = false;
String message;


IRsend irsend(irTransmitterPin);

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
}

void irSetup() {
  irsend.begin();
}

void setPin() {
  pinMode(PowerButtonPin, INPUT);
  pinMode(progUP, INPUT);
  pinMode(progDown, INPUT);
  pinMode(potiAnalogPin, INPUT);
  pinMode(powerLedPin, OUTPUT);
  digitalWrite(powerLedPin, LOW);
}

void loop() {
  int v = map(analogRead(potiAnalogPin), 0, 1023, 1000, 0);
  newVolume = v / 10;
  // Change Volume if Value is +- 2 (prevent toggle)
  if (newVolume >= (old_volume + 2) || newVolume <= (old_volume - 2)) {
    changeVolume(newVolume);
     delay(20);
  }

  // SwisscomTv on / off
  if (digitalRead(PowerButtonPin) == LOW && powerState == false) {
    digitalWrite(powerLedPin, HIGH);
    swisscomTvOn();
  }

  if (digitalRead(PowerButtonPin) == LOW && powerState == true) {
    digitalWrite(powerLedPin, HIGH);
    swisscomTvOff();
  }

  // Programm up/down
  if (digitalRead(progUP) == HIGH) {
    digitalWrite(powerLedPin, HIGH);
    progUPMethod();
  }

  if (digitalRead(progDown) == HIGH) {
    digitalWrite(powerLedPin, HIGH);
    progDownMethod();
  }

}

void progDownMethod() {
  Serial.println("prog Down");
  irsend.sendPronto(SwisscomProgDown, 26);
  digitalWrite(powerLedPin, LOW);
  delay(100);
  message = "iotdevicelogs,method=progDown,status=pressed value=0";
  createLog(message);
}

void progUPMethod() {
  Serial.println("prog UP");
  irsend.sendPronto(SwisscomProgUp, 26);
  digitalWrite(powerLedPin, LOW);
  delay(100);
  message = "iotdevicelogs,method=progUP,status=pressed value=0";
  createLog(message);
}

// Sonos Speaker vol
void changeVolume(int volume) {
  String fullpath = sonosSpeakerPath + String(volume);
  Serial.print("fullpath: ");
  Serial.println(fullpath);
  old_volume = volume;
  WiFiClient wifi;
  HttpClient client = HttpClient(wifi, sonosHost, sonosPort);
  client.get(fullpath);
  int statusCode = client.responseStatusCode();
  if (statusCode == 200) {
    message = "iotdevicelogs,method=changeVolume,status=ok value=" + String(volume);
    createLog(message);
  } else {
    message = "iotdevicelogs,method=changeVolume,status=failure value=-1";
    createLog(message);
  }
}

// Swisscom-TV ON
void swisscomTvOn() {
  Serial.println("Power");
  irsend.sendPronto(SwisscomPower, 28);
  powerState = true;
  digitalWrite(powerLedPin, LOW);
  delay(100);
  message = "iotdevicelogs,method=PowerOn,status=pressed value=1";
  createLog(message);
  menuOff();
}

// Swisscom-TV OFF
void swisscomTvOff() {
  Serial.println("Power");
  irsend.sendPronto(SwisscomPower, 28);
  digitalWrite(powerLedPin, LOW);
  powerState = false;
  delay(100);
  message = "iotdevicelogs,method=PowerOff,status=pressed value=0";
  createLog(message);
}

// Swisscom-TV Menu OFF
// Workaround swisscom TV turn menu on, when pressed the power button
void menuOff() {
  delay(1000);
  irsend.sendPronto(SwisscomMenu, 26);
}

void createLog(String logMessage) {
  String contentType = "application/x-www-form-urlencoded";
  WiFiClient wifi;
  HttpClient client = HttpClient(wifi, influxdbHost, influxdbPort);
  client.post(influxdbPath, contentType, logMessage);
}
