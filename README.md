# ESP32BLE-Sniffer-Stack

## Sniffer stack with esp32 and Docker

Esp32 sniffes for Ble --> Sends to Mqtt server ---> RSSI min/maxi/low 

and device count ---> dockercontainer writes into influxdb


## Getting Started

Upload esp32sniffer.ino with Huge APP partition scheme

edit username password...
```yaml
#define MQTT_server "brokerip"
#define MQTT_port 1883
#define MQTT_user "username"
#define MQTT_pass "password"
#define MQTT_name "Esp32"
```
and wifi credentials
```yaml
const char* ssid = "your wifi name"; // WIFIN-Name
const char* ssidkey = "your wifi password"; // WiFi Key
```

Run Docker 

replace the following variables

```yaml
docker run -d \
  --name='snifferstack' \
  -e HOST_CONTAINERNAME="snifferstack" \
  -e 'TOPIC'='ble/report' \
  -e 'MQTT_BROKER'='mqtt-broker' \
  -e 'INFLUX_HOST'='influx-ip' \
  -e 'INFLUX_PORT'='8086' \
  -e 'INFLUX_USER'='username' \
  -e 'INFLUX_PASS'='password' \
  -e 'INFLUX_DATABASE'='database' \
  pepaondrugs/esp32ble-sniffer-stack:latest
  ```


thanks to [ct-Open-Source](https://github.com/ct-Open-Source/esp32-bt-mqtt-scanner/blob/fb5163728d1f0f77dccba8ccc45dc972ff15c33b/Bluetooth-MQTT.ino) for the esp32 code
