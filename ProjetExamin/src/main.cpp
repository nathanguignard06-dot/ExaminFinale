#include <Arduino.h>
#include <WiFi.h>
#include <FS.h>
#include <SPIFFS.h>
#include <WebServer.h>
#include <HTTPClient.h>  // ← AJOUTE CET INCLUDE
#include <ArduinoJson.h> // Pour parser JSON facilement

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// declare an SSD1306 display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const char* ssid = "UNIFI_IDO1";
const char* password = "42Bidules!";
const char* apiToken = "a42485e1540c9b7f129254695a41d053";

WebServer server(80);

String fetchAndCleanData(const char* url, const char* fieldName) {
  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-Jsio-Token", apiToken);
  
  int httpCode = http.GET();
  
  if (httpCode == 200) {
    String payload = http.getString();
    
    StaticJsonDocument<500> doc;
    deserializeJson(doc, payload);
    
    if (doc.is<JsonArray>()) {
      JsonObject obj = doc[0];
      String value = obj[fieldName];
      value.replace("INVALID_EXPRESSION: ", "");
      return value;
    } else if (doc.is<JsonObject>()) {
      String value = doc[fieldName];
      value.replace("INVALID_EXPRESSION: ", "");
      return value;
    }
  }
  return "Erreur";
}

void handleTemperature() {
  String temp = fetchAndCleanData("https://api.jsonserver.io/donnees/temperature/collecter", "temperature");
  server.send(200, "application/json", "{\"temperature\": " + temp + "}");
}

void handleHumidity() {
  String humidity = fetchAndCleanData("https://api.jsonserver.io/donnees/humidity/collecter", "humidity");
  server.send(200, "application/json", "{\"humidity\": " + humidity + "}");
}

void handleLuminosity() {
  String luminosity = fetchAndCleanData("https://api.jsonserver.io/donnees/luminosity/collecter", "luminosity");
  server.send(200, "application/json", "{\"luminosity\": \"" + luminosity + "\"}");
}

void handleMouvement() {
  String mouvement = fetchAndCleanData("https://api.jsonserver.io/donnees/mouvement/collecter", "mouvement");
  server.send(200, "application/json", "{\"mouvement\": \"" + mouvement + "\"}");
}

void handleUserName() {
  String name = fetchAndCleanData("https://api.jsonserver.io/mes/donnees/collecter", "name");
  server.send(200, "application/json", "{\"name\": \"" + name + "\"}");
}

void handleUserDevice() {
  String device = fetchAndCleanData("https://api.jsonserver.io/user/device/info", "device");
  server.send(200, "application/json", "{\"device\": \"" + device + "\"}");
}

void handleUserEmail() {
  String email = fetchAndCleanData("https://api.jsonserver.io/user/email/data", "email");
  server.send(200, "application/json", "{\"email\": \"" + email + "\"}");
}

void handleRoot() {
  File file = SPIFFS.open("/index.html", "r");
  if (!file) 
  {
    server.send(404, "text/plain", "File Not Found");
    return;
  }

  server.streamFile(file, "text/html");
  file.close();
}

//variable de temp de reconection
int count = 0;

void setup() {
  Serial.begin(115200);
  
  // initialize OLED display with address 0x3C for 128x64
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 
  {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    if(count == 1)
    {
      oled.clearDisplay();
      oled.setTextSize(1);            // set text size
      oled.setTextColor(WHITE);       // set text color
      oled.setCursor(0, 10);          // set position to display
      oled.println();
      oled.println("Failed to connect...");
      oled.println("5 seconds before");
      oled.println("reconnection attempt");
      oled.display(); 
      delay(5000);
    }
    else if(count == 2)
    {
      oled.clearDisplay();
      oled.setTextSize(1);            
      oled.setTextColor(WHITE);       
      oled.setCursor(0, 10);          
      oled.println();
      oled.println("Failed to connect...");
      oled.println("10 seconds before");
      oled.println("reconnection attempt");
      oled.display(); 
      delay(10000);
    }
    else if(count >= 3)
    {
      oled.clearDisplay();
      oled.setTextSize(1);            
      oled.setTextColor(WHITE);       
      oled.setCursor(0, 10);          
      oled.println();
      oled.println("Failed to connect...");
      oled.println("15 seconds before");
      oled.println("reconnection attempt");
      oled.display(); 
      delay(15000);
    }

    count++;
  }

  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0, 10);
  oled.println();
  oled.println("Connected to WiFi");
  oled.println(WiFi.localIP());
  oled.display();

  Serial.print(WiFi.localIP());

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) 
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Define routes
  server.on("/", handleRoot);
  server.serveStatic("/style.css", SPIFFS, "/style.css");

  server.on("/api/temperature", handleTemperature);
  server.on("/api/humidity", handleHumidity);
  server.on("/api/luminosity", handleLuminosity);
  server.on("/api/mouvement", handleMouvement);
  server.on("/api/userName", handleUserName);
  server.on("/api/userDevice", handleUserDevice);
  server.on("/api/userEmail", handleUserEmail);

  // Start server
  server.begin();
}

void loop() 
{
  server.handleClient();
}