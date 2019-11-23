#include <ESP8266WiFi.h>

const char *ssid = "dlink_demo"; // TODO
const char *password = "iotsonospw"; // TODO

int VolumeUp = 15;
int VolumeDown = 16;
const char *host = "192.168.0.126";
const char *path = "/fernsehzimmer/volume/+5";
const int port = 5005;

void setup() {

  pinMode(VolumeUp, INPUT); 
  pinMode(VolumeDown, INPUT);

  
  Serial.begin(115200);
  Serial.print("\nConnecting to network ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100); // keeps watchdog happy
  }
  Serial.print("Connected to network, local IP = ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());
  // connect to remote host

}

void loop() {
  
   if (digitalRead(VolumeUp) == HIGH){
      WiFiClient client;
    if (client.connect(host, port)) {
     Serial.print("high");

    // send HTTP request
    client.print("GET ");
    client.print(path);
    client.print(" HTTP/1.1\r\n");
    client.print("Host: ");
    client.print(host);
    client.print("\r\n");
    client.print("Connection: close\r\n\r\n");

    // read HTTP response
    while (client.connected() || client.available()) {
      int ch = client.read();
      while (ch >= 0) {
        Serial.print((char) ch);
        ch = client.read();
      }
    }
  }
      
   }
  }
