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
const char* deviceId = "ROSETTE";
const char* mqttHost = "mqtt-host";
const int mqttPort = 12639;
const char* mqttLogin = "mqtt-login";
const char* mqttPass = "mqtt-pass";
const char* mqttTopic = "ROSETTE";
const int rosette1 = D0;
const int rosette2 = D5;
const int rosette3 = D6;
const int rosette4 = D7;

WiFiClient espClient;
PubSubClient client(espClient);

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
 
void setup() {
  Serial.begin(115200);
  client.setServer(mqttHost, mqttPort);
  client.setCallback(callback);
  pinMode(rosette1, OUTPUT);
  pinMode(rosette2, OUTPUT);
  pinMode(rosette3, OUTPUT);
  pinMode(rosette4, OUTPUT);
  
  digitalWrite(rosette1, HIGH);
  digitalWrite(rosette2, HIGH);
  digitalWrite(rosette3, HIGH);
  digitalWrite(rosette4, HIGH);
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
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  // const char* payloadDeviceId = doc["deviceId"];
  //if (payloadDeviceId != deviceId) {
    // return;
  // }
  const bool isRosette1On = doc["value"][0];
  const bool isRosette2On = doc["value"][1];
  const bool isRosette3On = doc["value"][2];
  const bool isRosette4On = doc["value"][3];

  if (isRosette1On) {
    digitalWrite(rosette1, LOW);
    Serial.println("state rosette1 true");
  } else {
    digitalWrite(rosette1, HIGH);
    Serial.println("state rosette1 false");
  }

  if (isRosette2On) {
    digitalWrite(rosette2, LOW);
    Serial.println("state rosette2 true");
  } else {
    digitalWrite(rosette2, HIGH);
    Serial.println("state rosette1 false");
  }

  if (isRosette3On) {
    digitalWrite(rosette3, LOW);
    Serial.println("state rosette3 true");
  } else {
    digitalWrite(rosette3, HIGH);
    Serial.println("state rosette1 false");
  }

  if (isRosette4On) {
    digitalWrite(rosette4, LOW);
    Serial.println("state rosette4 true");
  } else {
    digitalWrite(rosette4, HIGH);
    Serial.println("state rosette1 false");
  }

  Serial.println();
}
