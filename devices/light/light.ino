#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Pins
static const uint8_t D0   = 16;
static const uint8_t D1   = 5;
static const uint8_t D2   = 4;
static const uint8_t D3   = 0;
static const uint8_t D4   = 2;
static const uint8_t D5   = 14;
static const uint8_t D6   = 12;
static const uint8_t D7   = 13;
static const uint8_t D8   = 15;
static const uint8_t D9   = 3;
static const uint8_t D10  = 1;
 
// Settings
const char* ssid = "wifi-ssid";
const char* password = "q12345";
const char* deviceId = "room1-device3";
const char* mqttHost = "mqtt-host";
const int mqttPort = 12639;
const char* mqttLogin = "mqtt-login";
const char* mqttPass = "mqtt-pass";
const char* mqttTopic = "LIGHT_DEVICE-61d2bfd71d3d6e69f6f53141";
const byte ledPin = LED_BUILTIN;

WiFiClient espClient;
PubSubClient client(espClient);
 
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
 
void setup() {
  Serial.begin(9600);
  client.setServer(mqttHost, mqttPort);
  client.setCallback(callback);
  pinMode(D2, OUTPUT);
  connectToWifi();
}

void connectToWifi() {
  WiFi.begin(ssid, password);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}
 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Attempt to connect
    if (client.connect(deviceId)) {
      Serial.println("connected");
      client.subscribe(mqttTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  DynamicJsonDocument doc(1024);
  auto error = deserializeJson(doc, payload);
  if (error) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(error.c_str());
    return;
  }
  const char* payloadDeviceId = doc["deviceId"];
  if (payloadDeviceId != deviceId) {
    return;
  }
  const bool state = doc["value"];
  
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  if(state) {
    digitalWrite(D2, LOW);
    Serial.print("state true");
  } else {
    digitalWrite(D2, HIGH);
    Serial.print("state false");
  }
  Serial.println();
}
