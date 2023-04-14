#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "PBR";
const char* password = "veracity31";
const char* apiKey = "31757b9825c23fc55a1043ff48b17735";
const float lat = 47.185619;
const float lon = -95.940628;

const int BAUD_RATE = 9600;

const int requestInterval = 600000; // in milliseconds
int previousMillis = 600000;


const int SAMPLE_COUNT = 6; // Number of samples (1 per 10 minutes) for a 1-hour average

float windSpeedAvg = 0.0;
float windSpeedSamples[SAMPLE_COUNT];
int sampleIndex = 0;

void setup() {
  Serial.begin(BAUD_RATE);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  WiFi.config(IPAddress(192, 168, 1, 12), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
  for (int i = 0; i < SAMPLE_COUNT; i++) {
    windSpeedSamples[i] = 0;
  }
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= requestInterval) {
    previousMillis = currentMillis;
    updateWindSpeedAvg();
    }    
  Serial.println("Wind Speed Average: " + String(windSpeedAvg));
  delay(100);
}

void updateWindSpeedAvg() {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  String url = "https://api.openweathermap.org/data/2.5/weather?lat=" + String(lat) + "&lon=" + String(lon) + "&appid=" + apiKey + "&units=imperial";
  http.begin(client, url);
  if (http.GET() > 0) {
    delay(100);
    String jsonStr = http.getString();
    DynamicJsonDocument jsonDoc(1024);
    deserializeJson(jsonDoc, jsonStr);
    float windSpeed = jsonDoc["wind"]["speed"];
    Serial.println(windSpeed);
    delay(1000);

    JsonObject wind = jsonDoc["wind"];
    float windGust = 0;
    if (wind.containsKey("gust")) {
      windGust = wind["gust"];
      Serial.println(windGust);
    } else {
      Serial.println("No gust value");
    }
    delay(1000);

    float windSpeedCurrent;
    if (windGust == 0) {
      windSpeedCurrent = windSpeed;
    } else {
      windSpeedCurrent = (windSpeed + windGust) / 2.0;
    }
    
    windSpeedAvg -= windSpeedSamples[sampleIndex];
    windSpeedSamples[sampleIndex] = windSpeedCurrent;
    windSpeedAvg += windSpeedCurrent;
    sampleIndex = (sampleIndex + 1) % SAMPLE_COUNT;
    
    float currentAvg = windSpeedAvg;
    if (sampleIndex > 0) {
      currentAvg /= sampleIndex;
    } else {
      currentAvg /= SAMPLE_COUNT;
    }
    
    Serial.print("Current average wind speed: ");
    Serial.println(currentAvg);
  }
  http.end();
}
