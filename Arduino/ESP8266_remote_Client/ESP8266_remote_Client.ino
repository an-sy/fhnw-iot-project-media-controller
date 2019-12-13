#include <ESP8266WiFi.h>
#include <ArduinoHttpClient.h>
#include "secrets.h"

int potiAnalogPin = A0;
int newVolume = 0;
int old_volume;

void setup() {
  
  Serial.begin(115200);
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

void loop() {
  // map the Analog output to 100-0 
  newVolume = map(analogRead(potiAnalogPin), 0, 1023, 100, 0);
  // Change Volume if Value is +- 2 (prevent toggle) 
  if (newVolume >= (old_volume+2) || newVolume <= (old_volume-2)){
      changeVolume(newVolume);
   }
  }

void changeVolume(int volume) {
  String fullpath = path +String(volume);
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
  delay(100);
  }
