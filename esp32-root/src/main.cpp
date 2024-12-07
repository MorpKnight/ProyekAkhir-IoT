#include <WiFi.h>
#include <WebServer.h>

// ...existing code...

const char* ssid = "DESKTOP-5AJS436 4673";
const char* password = "081281083891";

WebServer server(80);

void handleRoot() {
  server.send(200, "text/plain", "Hello, world!");
}

void setup() {
  // ...existing code...

  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");

  // ...existing code...
}

void loop() {
  // ...existing code...
  server.handleClient();
  // ...existing code...
}
