#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAddress.h>


#define SLEEP_TIME 42
 
// MQTT-Parameter
#define MQTT_server "brokerip"
#define MQTT_port 1883
#define MQTT_user "username"
#define MQTT_pass "password"
#define MQTT_name "Esp32"
#define MQTT_scantopic "ble/report"
#define MQTT_statustopic "ble/stats"
#define QoS 0
 
const char* ssid = "your wifi name"; // WIFIN-Name
const char* ssidkey = "your wifi password"; // WiFi Key
 
int scanTime = 2; // Suchzeit
BLEScan* pBLEScan;
WiFiClient wclient;
PubSubClient mqtt(MQTT_server, MQTT_port, wclient);
 
void setup()
{
  int i;
  Serial.begin(115200);
  Serial.println("Booted");
  Serial.println("Warte auf WLAN-Verbindung");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, ssidkey);
  Serial.println("");
  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    timeout++;
    if  (timeout > 60) // Wenn Anmeldung nicht möglich
    {
      Serial.println("\r\nWLAN-Verbindung fehlt. Reboot.");
      ESP.restart(); // ESP32 neu starten
    }
  }
  Serial.println("");
  Serial.print("IP Addresse: ");
  Serial.println(WiFi.localIP());
 
  // ---------------------- MQTT ---------------------------------------------------
  if (mqtt.connect(MQTT_name, MQTT_user, MQTT_pass))
  {
    Serial.println("\r\nMQTT-Verbindung hergestellt");
    mqtt.setBufferSize(2048);
    mqtt.publish(MQTT_statustopic,"Geraet verbunden");
    mqtt.publish(MQTT_statustopic, String(WiFi.localIP()).c_str());
  }
 
  Serial.println("Starte BLE Scanner");
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);
}
 
void loop()
{
  int j;
  String jsonscanresult;
  BLEScanResults foundDevices = pBLEScan->start(scanTime);
 
  jsonscanresult = "{\"Results\":{";
 
  for (j = 0; j < foundDevices.getCount(); j++)
  {
    jsonscanresult = jsonscanresult + "\"" + foundDevices.getDevice(j).getAddress().toString().c_str() + "\":{";
    if (j != foundDevices.getCount() - 1) {
      if (foundDevices.getDevice(j).haveName()) {
         jsonscanresult = jsonscanresult + "\"name\":\"" + foundDevices.getDevice(j).getName().c_str() + "\","; 
      }
      else jsonscanresult = jsonscanresult + "\"name\":\"\",";

      if (foundDevices.getDevice(j).haveServiceUUID()) {
         jsonscanresult = jsonscanresult + "\"serviceuuid\":\"" + foundDevices.getDevice(j).getServiceUUID().toString().c_str() + "\","; 
      }
      else jsonscanresult = jsonscanresult + "\"serviceuuid\":\"\",";

      if (foundDevices.getDevice(j).haveManufacturerData()) {
         jsonscanresult = jsonscanresult + "\"servicemfdata\":\"" + foundDevices.getDevice(j).getName().c_str() + "\","; 
      }
      else jsonscanresult = jsonscanresult + "\"servicemfdata\":\"\",";
      
      jsonscanresult = jsonscanresult +  "\"rssi\":" + String(foundDevices.getDevice(j).getRSSI()) + "},";
    }
    else {
      if (foundDevices.getDevice(j).haveName()) {
         jsonscanresult = jsonscanresult + "\"name\":\"" + foundDevices.getDevice(j).getName().c_str() + "\","; 
      }
      else jsonscanresult = jsonscanresult + "\"name\":\"\",";

      if (foundDevices.getDevice(j).haveServiceUUID()) {
         jsonscanresult = jsonscanresult + "\"serviceuuid\":\"" + foundDevices.getDevice(j).getServiceUUID().toString().c_str() + "\","; 
      }
      else jsonscanresult = jsonscanresult + "\"serviceuuid\":\"\",";

      if (foundDevices.getDevice(j).haveManufacturerData()) {
         jsonscanresult = jsonscanresult + "\"servicemfdata\":\"" + foundDevices.getDevice(j).getName().c_str() + "\","; 
      }
      else jsonscanresult = jsonscanresult + "\"servicemfdata\":\"\",";
      
      jsonscanresult = jsonscanresult +  "\"rssi\":" + String(foundDevices.getDevice(j).getRSSI()) + "}";
    }
    
  }
  jsonscanresult = jsonscanresult + "}}";
  pBLEScan->clearResults(); // Speicher freigeben
  mqtt.publish(MQTT_scantopic, (char*) jsonscanresult.c_str());
  Serial.println(jsonscanresult);
  delay(100);

  
  if (WiFi.status() != WL_CONNECTED)  ESP.restart();
  if (mqtt.connected())
  {
    mqtt.loop();
  }
  else
  {
    Serial.println("MQTT Verbindung verloren");
    mqtt.connect(MQTT_name, MQTT_user, MQTT_pass);
    int timeout = 0;
    while (!mqtt.connected())
    {
     delay(500);
     Serial.print(".");
     timeout++;
     if  (timeout > 60) //
     {
       Serial.println("\r\nMQTT-Verbindung verloren. Reboot.");
       ESP.restart();
     }
    }

   mqtt.publish(MQTT_statustopic,"Geraet neu verbunden");
  }
  Serial.println("deep sleep started");
  delay(500);
  esp_deep_sleep(SLEEP_TIME * 1000000);
}
