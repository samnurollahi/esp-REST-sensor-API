#include <Arduino.h>
#include <ArduinoJson.h>

#include <Wire.h>
#include <WebServer.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#define WARN_LED_PIN 23
#define SENSOR_TEMP_PIN 4
#define LDR_PIN 12

const static String ssid = "kaka";
const static String password = "123456789";

WebServer server(80);

enum update
{
  LED,
  OLED,
  LDR,
  DS18B20
};

OneWire oneWire(SENSOR_TEMP_PIN);
DallasTemperature sensors(&oneWire);

unsigned long prevLdr = 0;
unsigned long prevOled = 0;
unsigned long prevTemp = 0;
int temp = 0;
int ldr = 0;
int led = 0;

float readTemp();
int readLdr();
void logicLed();
void updateFunc();

void setup()
{
  Serial.begin(9600);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/api/sensors", []()
            {
  String json = "{";
  json += "\"temperature\":" + String(temp) + ",";
  json += "\"ldr\":" + String(ldr);
  json += "}";

  server.send(200, "application/json", json); });

  server.on("/api/status", []()
            {
  String json = "{";
  json += "\"temperature\":" + String(temp) + ",";
  json += "\"ldr\":" + String(ldr) + ",";
  json += "\"led\":" + String(led);
  json += "}";

  server.send(200, "application/json", json); });

  server.on("/api/led", HTTP_POST, []()
            {
              String body = server.arg("plain");
              DynamicJsonDocument doc(1024);
              DeserializationError error = deserializeJson(doc, body);

              if (error)
              {
                server.send(
                    400,
                    "application/json",
                    "{\"error\":\"Invalid JSON\"}");
                return;
              }else {
                led = doc['brightness'];
                ledcWrite(0, led);

                server.send(
                    200,
                    "application/json",
                    "{\"message\":\"LED brightness updated\"}");
              } });

  server.begin();

  sensors.begin();

  pinMode(SENSOR_TEMP_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);
  pinMode(WARN_LED_PIN, OUTPUT);

  ledcSetup(0, 5000, 8);
  ledcAttachPin(WARN_LED_PIN, 0);
}

void loop()
{
  updateFunc();
  server.handleClient();
}

void updateFunc()
{
  unsigned long now = millis();

  if (now - prevLdr >= 200)
  {
    prevLdr = now;

    readLdr();
  }

  if (now - prevTemp >= 1000)
  {
    prevTemp = now;

    readTemp();
    logicLed();
  }
}

float readTemp()
{
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);
  return temp;
};
int readLdr()
{
  ldr = analogRead(LDR_PIN);
  ldr = map(ldr, 0, 4095, 0, 100);
  return ldr;
}

void logicLed()
{
  if (temp < 25)
  {
    led = 0;
    ledcWrite(0, 0);
  }
  else if (temp > 25 && temp < 30)
  {
    led = 76;
    ledcWrite(0, 76);
  }
  else if (temp > 30 && temp < 35)
  {
    led = 153;
    ledcWrite(0, 153);
  }
  else if (temp > 35)
  {
    led = 256;
    ledcWrite(0, 256);
  }
}