void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  // Create a String to hold the payload
  String payloadString = "";
  for (int i = 0; i < length; i++) {
    payloadString += (char)payload[i];
  }
  Serial.println(payloadString);

  if (payloadString == "relay1_on") {
    digitalWrite(relay1, LOW);
    Serial.println("Relay 1 ON");
  } else if (payloadString == "relay1_off") {
    digitalWrite(relay1, HIGH);
    Serial.println("Relay 1 OFF");
  } else if (payloadString == "relay2_on") {
    digitalWrite(relay2, LOW);
    Serial.println("Relay 2 ON");
  } else if (payloadString == "relay2_off") {
    digitalWrite(relay2, HIGH);
    Serial.println("Relay 2 OFF");
  } else if (payloadString == "relay3_on") {
    digitalWrite(relay3, LOW);
    Serial.println("Relay 3 ON");
  } else if (payloadString == "relay3_off") {
    digitalWrite(relay3, HIGH);
    Serial.println("Relay 3 OFF");
  } else if (payloadString == "relay4_on") {
    digitalWrite(relay4, LOW);
    Serial.println("Relay 4 ON");
  } else if (payloadString == "relay4_off") {
    digitalWrite(relay4, HIGH);
    Serial.println("Relay 4 OFF");
  }
}


void pubMessage() {
  lastMsg = millis();
  ++value;
  snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
  Serial.print("Publish message: ");
  Serial.println(msg);
  client.publish("outTopic", msg);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.publish(publish.c_str(), "ONLINE !");
      client.subscribe(subscribe.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
