#include <Arduino.h>

#include <ESP8266MQTTMesh.h>
#include <FS.h>
#include <Time.h>
#include <WiFiUdp.h>
#include <NTPClient.h>


#define      FIRMWARE_ID        0x1337
#define      FIRMWARE_VER       "0.1"


const char* mqtt_server           = "SERVER_IP";

int mqtt_port               = 1883;
const wifi_conn networks[]        = {
	 WIFI_CONN("WIFI_SSID", "WIFI_PASSWORD", "E0:41:36:1C:51:99", true),
        NULL
        };
const char*  mesh_password        = "TFM_mesh";


const char* nodeID ="DD";

unsigned long previousMillis = 0;
const long interval = 8000;
int cnt = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
const char* topicOut = "TFM/test/2/";
const char* topicIn = "TFM/test/2/";
char msg[50];
char formatedTime[20];


// Note: All of the '.set' options below are optional.  The default values can be
// found in ESP8266MQTTMeshBuilder.h
ESP8266MQTTMesh mesh = ESP8266MQTTMesh::Builder(networks, mqtt_server, mqtt_port)
                       .setVersion(FIRMWARE_VER, FIRMWARE_ID)
                       .setMeshPassword(mesh_password)
                       .setTopic(topicIn, topicOut)
                       .build();

void callback(const char* topic, const char* payload);

void setup() {
    Serial.begin(115200);
    delay(1000); //This is only here to make it easier to catch the startup messages.  It isn't required
    Serial.println("Setting time...");
    setTime(19,30,30,10,11,2019);
    timeClient.begin();
    // timeClient.setTimeOffset(3600);
    delay(1000);
    Serial.println("Setting callback...");
    mesh.setCallback(callback);
    delay(500);
    Serial.println("Setting mesh...");
    Serial.println(ESP.getFreeHeap());
    mesh.begin();
    delay(2000);
    Serial.println("Setup ok");
}

void loop() {
    if (! mesh.connected()){
        return;
			}
    unsigned long currentMillis = millis();
    String formatedDate = timeClient.getFormattedTime();
    if (currentMillis - previousMillis >= interval) {
          previousMillis = currentMillis;
          ++cnt;
          long rssi = WiFi.RSSI();
          formatedDate.toCharArray(formatedTime, 20);
          // node, number of message, send time, power rssi
          snprintf (msg, 50, "%s, %d, %6ld, %s, ", nodeID, cnt, rssi, formatedTime);
          mesh.publish(topicOut, msg, MSG_TYPE_NONE);
    }
}

void callback(const char* topic, const char* payload) {
    Serial.print("****,");
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
    Serial.print(payload);
    Serial.println();
}
