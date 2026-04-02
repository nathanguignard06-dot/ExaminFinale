#include <Arduino.h>
#include <WiFi.h>
#include <FS.h>
#include <SPIFFS.h>
#include <WebServer.h>

const char* ssid = "Famille Guignard";
const char* password = "R4683535";

WebServer server(80);

void handleRoot() {
  File file = SPIFFS.open("/index.html", "r");
  if (!file) {
    server.send(404, "text/plain", "File Not Found");
    return;
  }

  server.streamFile(file, "text/html");
  file.close();
}

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Define routes
  server.on("/", handleRoot);
  server.serveStatic("/style.css", SPIFFS, "/style.css");

  // Start server
  server.begin();
}

void loop() {
  server.handleClient();
}