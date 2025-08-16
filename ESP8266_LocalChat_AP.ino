#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>              // SPIFFS

// ---------- AP CONFIG ----------
const char* AP_SSID     = "SecretChat_8266";
const char* AP_PASSWORD = "chat8266"; // set "" for open AP

// ---------- SERVER & WS ----------
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Store a small rolling history so newcomers see recent messages
// (kept in RAM; cleared on reboot)
const size_t MAX_HISTORY = 50;
String history[MAX_HISTORY];
size_t historyCount = 0;

void addToHistory(const String& line) {
  if (line.length() == 0) return;
  if (historyCount < MAX_HISTORY) {
    history[historyCount++] = line;
  } else {
    // shift left (simple ring buffer)
    for (size_t i = 1; i < MAX_HISTORY; i++) history[i - 1] = history[i];
    history[MAX_HISTORY - 1] = line;
  }
}

void broadcast(const String& msg) {
  ws.textAll(msg);
  addToHistory(msg);
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
               AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    // Send recent history to the new client
    for (size_t i = 0; i < historyCount; i++) {
      client->text(history[i]);
    }
    // System join message for others
    String sys = "[system] A user joined.";
    broadcast(sys);
  }
  else if (type == WS_EVT_DISCONNECT) {
    String sys = "[system] A user left.";
    broadcast(sys);
  }
  else if (type == WS_EVT_DATA) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (!info->final || info->index != 0 || info->len != len) return;
    if (info->opcode == WS_TEXT) {
      // Data is a UTF-8 text frame; we expect simple pipe-delimited messages
      // Formats:
      //   JOIN|<name>
      //   MSG|<name>|<text>
      String payload;
      payload.reserve(len + 1);
      for (size_t i = 0; i < len; i++) payload += (char)data[i];

      // Very lightweight parsing (no extra JSON lib needed)
      if (payload.startsWith("JOIN|")) {
        String name = payload.substring(5);
        name.trim();
        if (name.length() == 0) name = "Anon";
        String joined = "[system] " + name + " joined the chat.";
        broadcast(joined);
      } else if (payload.startsWith("MSG|")) {
        int p1 = payload.indexOf('|', 4);
        if (p1 > 0) {
          String name = payload.substring(4, p1);
          String text = payload.substring(p1 + 1);
          name.trim(); text.trim();
          if (name.length() == 0) name = "Anon";
          if (text.length() > 0) {
            // Basic sanitation: remove CR/LF (UI also escapes HTML)
            text.replace("\r", " ");
            text.replace("\n", " ");
            String line = name + ": " + text;
            broadcast(line);
          }
        }
      }
    }
  }
}

void setupSPIFFS() {
  if (!SPIFFS.begin()) {
    // Try formatting if first mount fails (older chips)
    SPIFFS.format();
    if (!SPIFFS.begin()) {
      // Hard stop if still failing
      while (true) {
        delay(1000);
      }
    }
  }
}

void setupAP() {
  WiFi.mode(WIFI_AP);
  bool ok = WiFi.softAP(AP_SSID, AP_PASSWORD);
  delay(100); // give it a moment
}

void setupServer() {
  // WebSocket
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  // Serve static files from SPIFFS, default to index.html
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  // Basic health endpoint
  server.on("/health", HTTP_GET, [](AsyncWebServerRequest *req){
    req->send(200, "text/plain", "OK");
  });

  server.begin();
}

void setup() {
  // Small delay for stability on boot
  delay(500);

  setupSPIFFS();
  setupAP();
  setupServer();
}

void loop() {
  // Async server has no loop duties; keep WS alive
  ws.cleanupClients();
}
