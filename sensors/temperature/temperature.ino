#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

// Settings
const char* ssid = "wifi-ssid";
const char* password = "q12345";
const char* deviceId = "room1-device2";
const char* mqttHost = "192.168.0.5";
const int mqttPort = 1883;
const char* mqttTopic = "TEMPERATURE_SENSOR-61d2bfd71d3d6e69f6f53141";

#define DHTTYPE DHT11
#define DHTPIN  4
DHT dht(DHTPIN, DHTTYPE, 11);

// TODO:: get status from mqtt topic (nodejs app should send pereodical message with state of device to control loop)
bool isOnline = true;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);
  client.setServer(mqttHost, mqttPort);
  pinMode(4, INPUT);
  dht.begin();
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
  client.publish(mqttTopic, jsonString, true);
  Serial.println("Published!");
}

void loop() {
  if (isOnline == false) {
    return;
  }
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
    
  float temperatureFloat = dht.readTemperature();
  float humidityFloat = dht.readHumidity();
  
  String message = "";
  message.concat("{");
  message.concat("\"temperature\":");
  message.concat(temperatureFloat);
  message.concat(",");
  message.concat("\"humidity\":");
  message.concat(humidityFloat);
  message.concat("}");
  publishMessage(deviceId, message);
  Serial.println("-------");
  Serial.print("temperatureString - ");
  Serial.println(temperatureFloat);
  Serial.print("humidityString - ");
  Serial.println(humidityFloat);
  Serial.println(message);
  
  delay(5000);
}
