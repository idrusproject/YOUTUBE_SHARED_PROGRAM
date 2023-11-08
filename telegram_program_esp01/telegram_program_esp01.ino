#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// Wifi network station credentials
#define WIFI_SSID "Max 20A Unfused"
#define WIFI_PASSWORD "bonaparte"
// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "6565400541:AAFZQgOVpk0bx_bVbnwv_0Mn36bz0GamAUI"

#define relay 0

//X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);
unsigned long bot_lasttime;          // last time messages' scan has been done
bool Start = false;

String idBot = "898149240";
String idDevice = "lampu";

void setup() {
  Serial.begin(115200);
  Serial.println("Start");
  client.setInsecure();
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // attempt to connect to Wifi network:
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  Serial.print(" ");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  //client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  pinMode(1, OUTPUT);
  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(1, 0);
    delay(200);
    digitalWrite(1, 1);
    delay(200);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("WiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
  String startUp = idDevice + " Online ! .\n";
  bot.sendMessage(idBot, startUp);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, 1);
}

void loop() {
  if (millis() - bot_lasttime > 1000UL)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    bot_lasttime = millis();
  }
  delay(50);
}
