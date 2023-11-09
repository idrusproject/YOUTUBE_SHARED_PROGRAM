#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

// WiFi Credential
const char *ssid = "NAMA WIFI";
const char *password = "PASSWORD WIFI";

// MQTT Credential
const char *mqtt_broker = "broker.hivemq.com";
const char *mqtt_username = "";
const char *mqtt_password = "";
const int mqtt_port = 1883;

#define DHTPIN 32
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
float temp, hum;

// MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Task for GPS Dual Core
TaskHandle_t Task0;

void setup()
{
  Serial.begin(115200);
  wifiSetup();
  dht.begin();
  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
    mqttTask,   /* Task function. */
    "Task0",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &Task0,      /* Task handle to keep track of created task */
    1);          /* pin task to core 0 */
  delay(500);
}

// Network Task
void mqttTask( void * pvParameters ) {
  while (1) {
    Serial.print("Network Task running on core ");
    Serial.println(xPortGetCoreID());
    checkConnection();
    client.publish("esp32/tempidrus24", String(temp).c_str());
    client.publish("esp32/humidrus24", String(hum).c_str());
    client.loop();
    delay(100);
  }
}

void loop() {
  Serial.print("Main Task running on core ");
  Serial.println(xPortGetCoreID());
  getDht();
  delay(100);
}

void getDht() {
  temp = dht.readTemperature();
  hum = dht.readHumidity();
  Serial.print("Temp : " + String(temp));
  Serial.println(" Hum : " + String(hum));
}
