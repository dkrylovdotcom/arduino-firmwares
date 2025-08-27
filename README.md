# Arduino firmwares

Some firmwares that are used to test Smart Home ideas. ESP8266 & ESP32 is used for tests.

## Devices

- `./devices/camera-streaming`: camera streaming web server on ESP32Cam
- `./devices/light`: enable/disable light, using MQTT broker
- `./devices/qr-scanner`: QR code scanner using ESP32Cam & `ESP32QRCodeReader` lib. Transfers decoded data via MQTT topic
- `./devices/rosette`: enable/disable relay module using MQTT broker
- `./devices/switch`: enable/disable light, using MQTT broker & touch sensor

## Sensors

- `./sensors/light`: read analog light sensor
- `./sensors/motion`: detect motions using arduino motion sensor and send event to MQTT topic
- `./sensors/mq2`: detect CO2 gas via arduino sensor
- `./sensors/temperature`: collect temperature/humidity data using DHT sensor
- `./sensors/touch`: detect touch and send to MQTT topic
