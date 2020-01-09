// Copyright 2020 DYNAMO Consulting AB. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <SSD1306Wire.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DHT.h>

#error Place the network access here and comment this line
#define SSID      "<network ssid>"
#define PASSWORD  "<network password>"

// Resources
ESP8266WebServer server(80);
SSD1306Wire display(0x3c, 2, 14);
DHT dht(5, DHT11);

// Wifi statemachine states
enum WifiState {
  WIFI_INIT,
  WIFI_CONNECTING,
  WIFI_CONNECTED
};

// Globals
int counter = 0;
float humidity = 0;
float temperature = 0;
WifiState wifiState = WIFI_INIT;

// ======================================================================
// Wifi init mode and access
void wifi_init() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
}

// ======================================================================
// Root server response handler
void handleRoot() {
  server.send(200, "text/plain", "[Intentionally left blank]");
}

// ======================================================================
// Wifi start server when connected to wifi
void wifi_start() {
  Serial.println("");
  Serial.print("Connected: ");
  Serial.println(SSID);
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("miniweather")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/counter", []() {
    server.send(200, "text/plain", "Counter: " + String(counter));
  });

  server.on("/data", []() {
    server.send(200, "text/plain", "Temperature: " + String(temperature) + "\nHumidity: " + String(humidity));
  });

  server.begin();
  Serial.println("HTTP server started");
}

// ======================================================================
// Wifi statemachine
void wifi_manage() {
  switch (wifiState) {
  case WIFI_INIT:
    wifi_init();
    Serial.println("");
    wifiState = WIFI_CONNECTING;
    break;

  case WIFI_CONNECTING:
    if (WiFi.status() == WL_CONNECTED) {
      wifiState = WIFI_CONNECTED;
      wifi_start();
    }
    break;

  case WIFI_CONNECTED:
    server.handleClient();
    MDNS.update();
    break;

  default:
    break;
  }
}

// ======================================================================
void setup() {
  Serial.begin(115200);

  display.init();
  display.flipScreenVertically();

  dht.begin();
}

// ======================================================================
void loop() {
  static long nextCounterTime = 0;
  static long nextDHTTime = 0;
  bool update = false;
  long currentTime = millis();

  if (currentTime > nextCounterTime) {
    nextCounterTime += 100;
    counter += 1;
  }

  if (currentTime > nextDHTTime) {
    nextDHTTime += 5000;
    float _humidity = dht.readHumidity();
    float _temperature = dht.readTemperature();
    if (!isnan(_humidity) && !isnan(_temperature)) {
      humidity = _humidity;
      temperature = _temperature;
      update = true;
    }
  }

  if (update) {
    char str[15];
    display.clear();
    if (wifiState == WIFI_CONNECTED) {
      display.drawRect(0, 0, 128, 16);
    }
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_10);
    display.drawString(64, 2, "Mini Weather");
    if (wifiState == WIFI_CONNECTED) {
      display.drawRect(0, 16, 128, 32+16);
    }
    display.setFont(ArialMT_Plain_24);
    dtostrf(temperature, 5, 1, str);
    display.drawString(64, 26-11, String(str) + " C");
    dtostrf(humidity, 5, 1, str);
    display.drawString(64, 26+11, String(str) + " %");
    display.display();
  }

  wifi_manage();
}
