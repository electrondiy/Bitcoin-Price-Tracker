#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Make sure the I2C address is correct (it might be 0x3D as well)
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "SSID_NAME";
const char* password = "PASSWORD";
const char* api_url = "https://api.coindesk.com/v1/bpi/currentprice/BTC.json";

WiFiClientSecure client;

void setup() {
  Serial.begin(115200);

  // Initialize the display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for(;;); // Stop the program if there's an error
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.print("Connecting...");
  display.display();

  // Connect to Wi-Fi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // Settings for secure HTTPS connection
  client.setInsecure();
}

void loop() {
  Serial.println("Starting loop...");

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(client, api_url);
    Serial.println("HTTP request started...");

    int httpCode = http.GET();

    Serial.print("HTTP Code: ");
    Serial.println(httpCode);

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("Received Payload:");
      Serial.println(payload);

      StaticJsonDocument<1024> doc;
      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
      }

      const char* btc_price = doc["bpi"]["USD"]["rate"];
      Serial.print("BTC Price: ");
      Serial.println(btc_price);

      // Update the display
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 10);
      display.print("BTC Price:");
      display.display(); // Apply changes for the first text

      display.setCursor(0, 30);
      display.setTextSize(2);
      display.print(btc_price);
      display.display(); // Apply changes for the second text

      // Display the YouTube channel name at the bottom of the screen
      display.setTextSize(1.25);
      display.setCursor(24, 54);
      display.print("@Electron-DIY");
      display.display();

      Serial.println("Display updated");

    } else {
      Serial.println("Error on HTTP request");
    }

    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }

  delay(3000); // 3 seconds delay
}
