#include <Arduino.h>

#include <Wire.h>
#include <WebServer.h>

const static String ssid = "kaka";
const static String password = "123456789";

WebServer server(80);

void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


  server.on("/api/sensors", []() {
    
  });
  server.on("/api/status", []() {

  });
  server.on("/api/led", []() {

  });

  server.begin();
}

void loop() {
  server.handleClient();
}