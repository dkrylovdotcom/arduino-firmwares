#include <WiFi.h>
#include <ESP32QRCodeReader.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// See available models on README.md or ESP32CameraPins.h
ESP32QRCodeReader reader(CAMERA_MODEL_AI_THINKER);
 
// Settings
const char* ssid = "wifi-ssid";
const char* password = "q12345";
const char* deviceId = "outdoor-qr_scanner-1";
const char* mqttHost = "mqtt-host";
const int mqttPort = 12639;
const char* mqttLogin = "mqtt-login";
const char* mqttPass = "mqtt-pass";
const char* mqttTopic = "QR_SCANNER";

const char* qrCodePayload;

WiFiClient espClient;
PubSubClient client(espClient);
 
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
delay(10000);
}
 
void setup() {
  Serial.begin(115200);
  client.setServer(mqttHost, mqttPort);
  connectToWifi();
  startQRCodeScanning();
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

void startQRCodeScanning() {
  reader.setup();
  reader.beginOnCore(1);
  xTaskCreate(onQrCodeTask, "onQrCode", 4 * 1024, NULL, 4, NULL);
}

void onQrCodeTask(void *pvParameters) {
  struct QRCodeData qrCodeData;

  while (true) {
    if (reader.receiveQrCode(&qrCodeData, 100)) {
      Serial.println("Found QRCode");
      if (qrCodeData.valid) {
        qrCodePayload = (const char *) qrCodeData.payload;

        Serial.print("Payload: ");
        Serial.println(qrCodePayload);

        String message = "";
        // message.concat("{");
        // message.concat("\"value\":");
        message.concat(qrCodePayload);
        // message.concat("}");

        publishMessage(deviceId, qrCodePayload);

        Serial.println("-------");
        Serial.println(message);

      } else {
        Serial.print("Invalid: ");
        Serial.println((const char *)qrCodeData.payload);
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
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
