#include <Arduino.h>

#include <ESP8266MQTTMesh.h>
#include <FS.h>
#include <Time.h>
#include <WiFiUdp.h>
#include <NTPClient.h>


#define      FIRMWARE_ID        0x1337
#define      FIRMWARE_VER       "0.1"

#define FLAME A0
#define BUZZER D8
#define LED D0


const char* mqtt_server           = "SERVER_IP";
// const char* mqtt_server = "SERVER_IP";
int mqtt_port               = 1883;
const wifi_conn networks[]        = {
	WIFI_CONN("WIFI_SSID", "WIFI_PASSWORD", "E0:41:36:1C:51:99", true),
        NULL
        };
const char*  mesh_password        = "TFM_mesh";


const char* nodeID ="EE";

unsigned long previousMillis = 0;
const long interval = 8000;
int cnt = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
const char* topicOut = "TFM/home/sr/f/";
const char* topicIn = "TFM/home/sr/f/";
char msg[50];
char formatedTime[20];

int fire_detected ;


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
  pinMode(FLAME, INPUT);
  pinMode(BUZZER, OUTPUT);
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

    fire_detected = analogRead(FLAME) ;
    delay(500);

    if (fire_detected < 100)
    {
      Serial.print(fire_detected);
      Serial.println(" Fire detected");
      digitalWrite(BUZZER, HIGH);
      digitalWrite(LED, LOW);

      // Send message to broker
      formatedDate.toCharArray(formatedTime, 20);
      // node, number of message, send time, power rssi
      snprintf (msg, 50, "ALERT, %s, Fire, detected, %s", nodeID, formatedTime);
      mesh.publish(topicOut, msg, MSG_TYPE_NONE);
      mesh.publish_node("TFM/home/pl/in/", msg, MSG_TYPE_NONE);
      delay(5000);
    }
    else
    {
      Serial.print(fire_detected);
      Serial.println(" Fire not detected");
      digitalWrite(LED, HIGH);
      digitalWrite(BUZZER, LOW);
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
