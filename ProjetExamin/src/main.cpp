#include <Arduino.h>
#include <WiFi.h>
#include <FS.h>
#include <SPIFFS.h>
#include <WebServer.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// declare an SSD1306 display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const char* ssid = "UNIFI_IDO1";
const char* password = "42Bidules!";

WebServer server(80);

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
    if(count = 1)
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
    else if(count = 2)
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

  // Start server
  server.begin();
}

void loop() 
{
  server.handleClient();
}