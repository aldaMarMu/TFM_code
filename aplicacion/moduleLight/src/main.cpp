#include <Arduino.h>

#include <ESP8266MQTTMesh.h>
#include <FS.h>
#include <Time.h>
#include <WiFiUdp.h>
#include <NTPClient.h>


#define      FIRMWARE_ID        0x1337
#define      FIRMWARE_VER       "0.1"

#define SENSOR A0
#define LED D0

const char* mqtt_server           = "SERVER_IP";
// const char* mqtt_server = "SERVER_IP";
int mqtt_port               = 1883;
const wifi_conn networks[]        = {
	WIFI_CONN("WIFI_SSID", "WIFI_PASSWORD", "E0:41:36:1C:51:99", true),
        NULL
        };
const char*  mesh_password        = "TFM_mesh";


const char* nodeID ="BB";

unsigned long previousMillis = 0;
const long interval = 8000;
int cnt = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
const char* topicOut = "TFM/home/sr/l/";
const char* topicIn = "TFM/home/sr/l/";
char msg[50];
char formatedTime[20];

int light_detected ;


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
    timeClient.begin();
    delay(1000);
    pinMode(SENSOR, INPUT);
    pinMode(LED, OUTPUT);
    delay(500);
    Serial.println("finished Hardware set up");
    Serial.println("Setting callback...");
    mesh.setCallback(callback);
    delay(500);
    Serial.println("Setting mesh...");
		Serial.println(ESP.getFreeHeap());
    delay(1000);
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

    light_detected = analogRead(SENSOR) ;
    delay(500);

    if (light_detected > 300)
    {
      Serial.print(light_detected);
      Serial.println(" Darkness");

      formatedDate.toCharArray(formatedTime, 20);
      snprintf (msg, 50, "%s, light, %d, %s",nodeID, light_detected, formatedTime);
      mesh.publish(topicOut, msg, MSG_TYPE_NONE);
      mesh.publish_node("TFM/home/pl/in/", msg, MSG_TYPE_NONE);
      delay(5000);
    }
    else
    {
      Serial.print(light_detected);
      Serial.println(" Lightness");
      // Send message to broker
      formatedDate.toCharArray(formatedTime, 20);
      snprintf (msg, 50, "%s, light, %d, %s",nodeID, light_detected, formatedTime);
      mesh.publish(topicOut, msg, MSG_TYPE_NONE);
      mesh.publish_node("TFM/home/pl/in/", msg, MSG_TYPE_NONE);
      delay(5000);
    }
    delay(500);
}

void callback(const char* topic, const char* payload) {
	Serial.println("************************************");
  Serial.print(topic);
  Serial.print("  ");
	Serial.println(payload);
  Serial.println("************************************");
}
