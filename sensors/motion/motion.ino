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
const char* deviceId = "6e69f6f5314161d2bfd71d3d";
const char* mqttHost = "mqtt-host";
const int mqttPort = 12639;
const char* mqttLogin = "mqtt-login";
const char* mqttPass = "mqtt-pass";
const char* mqttTopic = "api.devices.data.output";
const int serialSpeed = 9600;

const int loopDelay = 1000;
const int motionDetectedDelay = 10000;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void setup() {
  Serial.begin(9600);

  connectToWifi();
  connectMQTT();

  pinMode(D2, INPUT);
}
 
void loop() {
  Serial.println("-----");
  bool isMotionDetected = motionRead();
  
  if(isMotionDetected) {
    publishMessage(deviceId, "1");
    Serial.println("Motion detected!");
    delay(motionDetectedDelay);
  }

  if (!mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop();
  delay(loopDelay);
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

void publishMessage(String deviceId, String value) {
  DynamicJsonDocument doc(200);
  char jsonString[200];
  doc["deviceId"] = deviceId;
  doc["state"] = value;
  serializeJson(doc, jsonString);
  mqttClient.publish(mqttTopic, jsonString, true);
  Serial.println("Published!");
  Serial.println(jsonString);
}

bool motionRead() {
  if(digitalRead(D4) == HIGH) {
    int motion = digitalRead(D4);
    Serial.println(motion);
    return true;
  } else {
    Serial.println("no trigger");
    return false;
  }
}
