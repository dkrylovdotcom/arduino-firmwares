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
const char* deviceId = "f6f5314161d2b6e69fd71d3d";
const char* mqttHost = "mqtt-host";
const int mqttPort = 12639;
const char* mqttLogin = "mqtt-login";
const char* mqttPass = "mqtt-pass";
const char* mqttTopic = "api.devices.data.input";
const int serialSpeed = 9600;

struct deviceInfo {
  String name = "switch";
};

int buttonState = 0;
int mainLoopDelay = 1000;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void setup() {
  Serial.begin(serialSpeed);

  connectToWifi();
  connectMQTT();

  pinMode(D2, INPUT);
}
 
void loop() {
  if (!mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop();

  handleTouch();
  delay(mainLoopDelay);
}

void connectToWifi() {
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid, password);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void connectMQTT() {
  mqttClient.setServer(mqttHost, mqttPort);
  // mqttClient.setCallback(callback);

  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Attempt to connect
    if (mqttClient.connect(deviceId, mqttLogin, mqttPass)) {
      Serial.println("connected");
      mqttClient.subscribe(mqttTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void handleTouch() {
  int data = digitalRead(D2);
  if (data != 1) {
    return;
  }

  if (buttonState == 1) {
    buttonState = 0;
  } else {
    buttonState = 1;
  }
  String message = generateJsonMessage(buttonState);
  Serial.println("Publish: " + message);

  mqttClient.publish(mqttTopic, message.c_str());
}

String generateJsonMessage(int state) {
  String message = "";
  message.concat("{");
  message.concat("\"state\":");
  message.concat(state);
  message.concat("}");
  return message;
}
