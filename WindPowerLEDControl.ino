#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Put your SSID";
const char* password = "put your password here";
const char* apiKey = "put ypur API key here";
const float lat = Put you lat here;
const float lon = Put you lon here;

const int LED_PINS[] = {D0, D1, D2, D3, D4, D5};
const int NUM_LEDS = sizeof(LED_PINS) / sizeof(LED_PINS[0]);

const int BAUD_RATE = 9600;
const int DELAY_TIME = 3600000;

void setup() {
  Serial.begin(BAUD_RATE);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  WiFi.config(IPAddress(192, 168, 1, 12), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
  }
}

void loop() {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  String url = "https://api.openweathermap.org/data/2.5/weather?lat=" + String(lat) + "&lon=" + String(lon) + "&appid=" + apiKey + "&units=imperial";
  http.begin(client, url);
  if (http.GET() > 0) {
    String jsonStr = http.getString();
    DynamicJsonDocument jsonDoc(1024);
    deserializeJson(jsonDoc, jsonStr);
    float windSpeed = jsonDoc["wind"]["speed"];
    Serial.println("Wind Speed: " + String(windSpeed));
    if (windSpeed < 14) {
      for (int i = 0; i < NUM_LEDS; i++) {
        digitalWrite(LED_PINS[i], HIGH);
      }
      Serial.println("0 Rigs are running");
    } else if (windSpeed < 15) {
      digitalWrite(LED_PINS[0], LOW);
      digitalWrite(LED_PINS[1], HIGH);
      digitalWrite(LED_PINS[2], HIGH);
      digitalWrite(LED_PINS[3], HIGH);
      digitalWrite(LED_PINS[4], HIGH);
      digitalWrite(LED_PINS[5], HIGH);;
      Serial.println("1 Rigs are running");
    } else if (windSpeed < 17) {
      digitalWrite(LED_PINS[0], LOW);
      digitalWrite(LED_PINS[1], LOW);
      digitalWrite(LED_PINS[2], HIGH);
      digitalWrite(LED_PINS[3], HIGH);
      digitalWrite(LED_PINS[4], HIGH);
      digitalWrite(LED_PINS[5], HIGH);
      Serial.println("2 Rigs are running");
    } else if (windSpeed < 18) {
      digitalWrite(LED_PINS[0], LOW);
      digitalWrite(LED_PINS[1], LOW);
      digitalWrite(LED_PINS[2], LOW);
      digitalWrite(LED_PINS[3], HIGH);
      digitalWrite(LED_PINS[4], HIGH);
      digitalWrite(LED_PINS[5], HIGH);
      Serial.println("3 Rigs are running");
    } else if (windSpeed < 19) {
      digitalWrite(LED_PINS[0], LOW);
      digitalWrite(LED_PINS[1], LOW);
      digitalWrite(LED_PINS[2], LOW);
      digitalWrite(LED_PINS[3], LOW);
      digitalWrite(LED_PINS[4], HIGH);
      digitalWrite(LED_PINS[5], HIGH);
      Serial.println("4 Rigs are running");
    } else if (windSpeed < 20) {
      digitalWrite(LED_PINS[0], LOW);
      digitalWrite(LED_PINS[1], LOW);
      digitalWrite(LED_PINS[2], LOW);
      digitalWrite(LED_PINS[3], LOW);
      digitalWrite(LED_PINS[4], LOW);
      digitalWrite(LED_PINS[5], HIGH);           
      Serial.println("5 Rigs are running");
    } else {
      digitalWrite(LED_PINS[0], LOW);
      digitalWrite(LED_PINS[1], LOW);
      digitalWrite(LED_PINS[2], LOW);
      digitalWrite(LED_PINS[3], LOW);
      digitalWrite(LED_PINS[4], LOW);
      digitalWrite(LED_PINS[5], LOW);
      Serial.println("6 Rigs are running");
    }
  }
  http.end();
  delay(DELAY_TIME);
}

