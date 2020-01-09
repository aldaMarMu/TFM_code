#include <Arduino.h>

#include <ESP8266MQTTMesh.h>
#include <FS.h>
#include <Time.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#include "DHT.h"

#define      FIRMWARE_ID        0x1337
#define      FIRMWARE_VER       "0.1"

#define DHTTYPE DHT11   // DHT 11
#define SENSOR D3
#define LED D0

const char* mqtt_server           = "SERVER_IP";
// const char* mqtt_server = "SERVER_IP";
int mqtt_port               = 1883;
const wifi_conn networks[]        = {
	WIFI_CONN("WIFI_SSID", "WIFI_PASSWORD", "E0:41:36:1C:51:99", true),
        NULL
        };
const char*  mesh_password        = "TFM_mesh";


const char* nodeID ="FF";

unsigned long previousMillis = 0;
const long interval = 8000;
int cnt = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
const char* topicOut = "TFM/home/sr/h/";
const char* topicIn = "TFM/home/sr/h/";
char msg[50];
char formatedTime[20];

int h, t ;

DHT dht(SENSOR, DHTTYPE);


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
    dht.begin();
    Serial.println("Setting time...");
    timeClient.begin();
    delay(1000);
    // pinMode(SENSOR, INPUT);
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

    int err;

    h = dht.readHumidity();
    t = dht.readTemperature();
    
    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    Serial.print("Temperatura: ");
    Serial.print(t);
    Serial.print(" Humedad: ");
    Serial.print(h);
    Serial.println();

    
    delay(500);
    if (18 < t  && t < 23)
    {
      digitalWrite(LED, LOW);

      // Send message to broker
      formatedDate.toCharArray(formatedTime, 20);
      snprintf (msg, 50, "%s, temperature, %d, %s",nodeID, t, formatedTime);
      mesh.publish(topicOut, msg, MSG_TYPE_NONE);
      mesh.publish_node("TFM/home/pl/in/", msg, MSG_TYPE_NONE);
      delay(5000);
    }    else
    {
      long rssi = WiFi.RSSI();
      formatedDate.toCharArray(formatedTime, 20);
      snprintf (msg, 50, "ALERT, %s, temperature, %d, %s",nodeID, t, formatedTime);
      mesh.publish(topicOut, msg, MSG_TYPE_NONE);
      mesh.publish_node("TFM/home/pl/in/", msg, MSG_TYPE_NONE);
      delay(5000);
    }
    if(35 < h  && h < 60){
      formatedDate.toCharArray(formatedTime, 20);
      snprintf (msg, 50, "%s, hummidity, %d, %s",nodeID, h, formatedTime);
      mesh.publish(topicOut, msg, MSG_TYPE_NONE);
      mesh.publish_node("TFM/home/pl/in/", msg, MSG_TYPE_NONE);
      delay(5000);
    }
    else
    {
      formatedDate.toCharArray(formatedTime, 20);
      snprintf (msg, 50, "ALERT, %s, hummidity, %d, %s",nodeID, h, formatedTime);
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
