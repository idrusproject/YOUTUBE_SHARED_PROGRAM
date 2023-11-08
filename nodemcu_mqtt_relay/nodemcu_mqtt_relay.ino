#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define relay1 D3
#define relay2 D4
#define relay3 D6
#define relay4 D7

const char* ssid = "NAMA WIFI";
const char* password = "PASSWORD WIFI";
const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

String publish = "idrusproject/msg";
String subscribe = "idrusproject/relays";

void setup() {
  Serial.begin(115200);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  digitalWrite(relay1, 1);
  digitalWrite(relay2, 1);
  digitalWrite(relay3, 1);
  digitalWrite(relay4, 1);
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  delay(100);
}
