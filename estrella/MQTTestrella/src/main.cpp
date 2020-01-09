#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Time.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// Update these with values suitable for your network.

const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";
const char* mqtt_server = "SERVER_IP";

const char* out_topic="TFM/test/1";
const char* in_topic="TFM/test/1";

const char* nodeID="EE";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
char formatedTime[20];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

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
  long rssi = WiFi.RSSI();
  String formattedDate = timeClient.getFormattedTime();

  // [topic], RX node, power rssi, RX time, message
  Serial.print("[");
  Serial.print(topic);
  Serial.print("], ");
  Serial.print(nodeID);
  Serial.print(", ");
  Serial.print(rssi);
  Serial.print(", ");
  Serial.print(formattedDate);
  Serial.print(", ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      char msgRes[50];
      // Once connected, publish an announcement...
      snprintf (msgRes, 20, "%s, hello world test", nodeID);
      client.publish(out_topic, msgRes);
      // ... and resubscribe
      client.subscribe(in_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  setTime(19,30,30,10,11,2019);
  Serial.begin(115200);
  setup_wifi();
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(3600);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  client.loop();

  String formatedDate = timeClient.getFormattedTime();
  long now = millis();
  if (now - lastMsg > 8000) {
    lastMsg = now;
    ++value;
    long rssi = WiFi.RSSI();
    formatedDate.toCharArray(formatedTime, 20);
    // node, number of message, send time, power rssi
    snprintf (msg, 50, "%s, %d, %6ld, %s, ", nodeID, value, rssi, formatedTime);
    // Serial.print("Publish message: ");
    // Serial.println(msg);
    client.publish(out_topic, msg);
  }
}
