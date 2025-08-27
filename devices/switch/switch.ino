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
 
// Settings (this settings should be come from khome-display app after auth in guest wifi network)
const char* ssid = "wifi-ssid";
const char* password = "q12345";
const char* deviceId = "61d2bfd71d3d6e69f6f53141";
const char* mqttHost = "mqtt-host";
const int mqttPort = 12639;
const char* mqttLogin = "mqtt-login";
const char* mqttPass = "mqtt-pass";
const char* mqttTopic = "api.devices.data.input";
const int serialSpeed = 9600;

struct deviceInfo {
  String name = "switch";
};

int buttonState = false;
int switchState = false;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void setup() {
  Serial.begin(serialSpeed);

  connectToWifi();
  connectMQTT();

  pinMode(D1, INPUT);
  pinMode(D2, OUTPUT);
}
 
void loop() {
  handleTouch();

  if (!mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop();
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
  mqttClient.setCallback(callback);

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

  const char* x = doc["state"];
  Serial.println(x);

  // const char* payloadDeviceId = doc["deviceId"];
  //if (payloadDeviceId != deviceId) {
    // return;
  // }
  const bool isOn = doc["state"];

  if (isOn) {
    switchOn();
  } else {
    switchOff();
  }

  Serial.println();
}

void handleTouch() {
  bool isTouched = recognizeTouch();

  if (isTouched) {
    if (!switchState) {
      switchOn();
    } else {
      switchOff();
    }
  }
}

void switchOn() {
  digitalWrite(D2, HIGH);
  switchState = true;
  Serial.println("switchOn");
}

void switchOff() {
  digitalWrite(D2, LOW);
  switchState = false;
  Serial.println("switchOff");
}

bool recognizeTouch() {
  int data = digitalRead(D1);
  Serial.println(data);
  if (data == 1) {
    if (!buttonState) {
      buttonState = true;
      Serial.println("Нажата");

      String message = generateJsonMessage(buttonState);
      Serial.println("Publish: " + message);

      return true;
    }
  } else {
    if (buttonState) {
      buttonState = false;
      Serial.println("Не нажата");

      String message = generateJsonMessage(buttonState);
      Serial.println("Publish: " + message);
    }
  }

  return false;
}

String generateJsonMessage(bool state) {
  String message = "";
  message.concat("{");
  message.concat("\"state\":");
  message.concat(state);
  message.concat("}");
  return message;
}
