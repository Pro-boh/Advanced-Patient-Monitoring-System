#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Pixel_3125";
const char* password = "promaxpro";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
}

#include <HTTPClient.h>

void loop() {
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://example.com/data");
    int httpCode = http.GET();

    if(httpCode > 0) {
      String payload = http.getString();
      Serial.println(payload);
    }

    http.end();
  }
  delay(10000);

  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://example.com/data");
    int httpCode = http.GET();

    if(httpCode > 0) {
      String payload = http.getString();
      Serial.println(payload);
    }

    http.end();
  }
  delay(10000);
}
