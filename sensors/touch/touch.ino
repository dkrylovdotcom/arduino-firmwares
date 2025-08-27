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
const char* deviceId = "room1-device5";
const char* mqttHost = "mqtt-host";
const int mqttPort = 12639;
const char* mqttLogin = "mqtt-login";
const char* mqttPass = "mqtt-pass";
const char* mqttTopic = "TOUCH_SENSOR-61d2bfd71d3d6e69f6f53141";
int mqttPubQos = 2;
int currentState = false;
bool mqttRetain = true;

WiFiClient espClient;
PubSubClient client(espClient);
 
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  int data = digitalRead(D2);
  if (data == 1){
    if (!currentState) {
      currentState = true;
      Serial.println("Нажата");

      String message = getMessage(currentState);
      Serial.println("Publish: " + message);
      publishMessage(deviceId, message);
    }
  } else{
    if (currentState) {
      currentState = false;
      Serial.println("Не нажата");

      String message = getMessage(currentState);
      Serial.println("Publish: " + message);
      publishMessage(deviceId, message);
    }
  }
}
 
void setup() {
  Serial.begin(9600);
  client.setServer(mqttHost, mqttPort);
  pinMode(D2, INPUT);
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
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void publishMessage(String deviceId, String value) {
  DynamicJsonDocument doc(200);
  char jsonString[200];
  doc["deviceId"] = deviceId;
  doc["value"] = value;
  serializeJson(doc, jsonString);
  client.publish(mqttTopic, jsonString, mqttRetain);
  // client.publish(mqttTopic, (const uint8_t*)jsonString, mqttPubQos, mqttRetain);
  Serial.println("Published!");
}

String getMessage(bool state) {
  String message = "";
  message.concat("{");
  message.concat("\"state\":");
  message.concat(state);
  message.concat("}");
  return message;
}
