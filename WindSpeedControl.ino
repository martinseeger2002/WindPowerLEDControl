#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "YOUR SSID";
const char* password = "YOUR WIFI PASSWORD";
const char* apiKey = "YOUR API KEY";
const float lat = 00.0000;
const float lon = 00.0000;

const int LED_PINS[] = {D0, D1, D2, D3, D4, D5};
const int NUM_LEDS = sizeof(LED_PINS) / sizeof(LED_PINS[0]);

const int BAUD_RATE = 9600;

const int requestInterval = 6000; // in milliseconds
int previousMillis = 6000;


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
  
  // Set LED_PINS as output pins
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
  }
  
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
  controlRigs();
  delay(1000);
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

void controlRigs() {
  
  if (windSpeedAvg < 14) {
    digitalWrite(LED_PINS[0], HIGH);
    digitalWrite(LED_PINS[1], HIGH);
    digitalWrite(LED_PINS[2], HIGH);
    digitalWrite(LED_PINS[3], HIGH);
    digitalWrite(LED_PINS[4], HIGH);
    digitalWrite(LED_PINS[5], HIGH);
    Serial.println("0 Rigs are running");
  } else if (windSpeedAvg < 15) {
    digitalWrite(LED_PINS[0], LOW);
    digitalWrite(LED_PINS[1], HIGH);
    digitalWrite(LED_PINS[2], HIGH);
    digitalWrite(LED_PINS[3], HIGH);
    digitalWrite(LED_PINS[4], HIGH);
    digitalWrite(LED_PINS[5], HIGH);
    Serial.println("1 Rigs are running");
  } else if (windSpeedAvg < 17) {
    digitalWrite(LED_PINS[0], LOW);
    digitalWrite(LED_PINS[1], LOW);
    digitalWrite(LED_PINS[2], HIGH);
    digitalWrite(LED_PINS[3], HIGH);
    digitalWrite(LED_PINS[4], HIGH);
    digitalWrite(LED_PINS[5], HIGH);
    Serial.println("2 Rigs are running");
  } else if (windSpeedAvg < 18) {
    digitalWrite(LED_PINS[0], LOW);
    digitalWrite(LED_PINS[1], LOW);
    digitalWrite(LED_PINS[2], LOW);
    digitalWrite(LED_PINS[3], HIGH);
    digitalWrite(LED_PINS[4], HIGH);
    digitalWrite(LED_PINS[5], HIGH);
    Serial.println("3 Rigs are running");
  } else if (windSpeedAvg < 19) {
    digitalWrite(LED_PINS[0], LOW);
    digitalWrite(LED_PINS[1], LOW);
    digitalWrite(LED_PINS[2], LOW);
    digitalWrite(LED_PINS[3], LOW);
    digitalWrite(LED_PINS[4], HIGH);
    digitalWrite(LED_PINS[5], HIGH);
    Serial.println("4 Rigs are running");
  } else if (windSpeedAvg < 20) {
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

