#include <Arduino.h>

#include <ESP8266MQTTMesh.h>
#include <FS.h>
#include <Time.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <U8g2lib.h>


#define      FIRMWARE_ID        0x1337
#define      FIRMWARE_VER       "0.1"

#define BUTTON_OK D3
#define BUTTON_CANCEL D2
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

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ 16, /* clock=*/ 5, /* data=*/ 4);

unsigned long previousMillis = 0;
const long interval = 8000;
int cnt = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
const char *out_topic = "TFM/home/pl/out/";
const char *in_topic = "TFM/home/pl/in/";
char msg[50];
char formatedTime[20];

int motion_detected ;
boolean alert = false;
String msg_rcv ="test";
char msg_rcv_send[50];


// Note: All of the '.set' options below are optional.  The default values can be
// found in ESP8266MQTTMeshBuilder.h
ESP8266MQTTMesh mesh = ESP8266MQTTMesh::Builder(networks, mqtt_server, mqtt_port)
                       .setVersion(FIRMWARE_VER, FIRMWARE_ID)
                       .setMeshPassword(mesh_password)
                       .setTopic(in_topic, out_topic)
                       .build();

void callback(const char *topic, const char *msg);
void writeInScreen(const char *msg);

void setup() {
  Serial.begin(115200);
  Serial.println("Setting callback...");
  mesh.setCallback(callback);
  mesh.begin();

  delay(1000); //This is only here to make it easier to catch the startup messages.  It isn't required
  Serial.println("Setting time...");
  timeClient.begin();
  delay(1000);
  pinMode(BUTTON_OK, INPUT);
  pinMode(BUTTON_CANCEL, INPUT);
  pinMode(LED, OUTPUT);

  writeInScreen("Esperando...");
  delay(1000);

  Serial.println("finished Hardware set up");

  Serial.println(" Setup ok");
}

void loop() {
    if (! mesh.connected()){
        return;
			}
    String formatedDate = timeClient.getFormattedTime();
    if(alert){
    if(digitalRead(BUTTON_OK) == LOW){
        alert = true;
        formatedDate.toCharArray(formatedTime, 20);
        Serial.println("pulsado boton ok");
        snprintf (msg, 50, "OK, %s, %s", msg_rcv_send, formatedTime);
        mesh.publish(out_topic, msg, MSG_TYPE_NONE); 
        writeInScreen("OK, enviando...");
        delay(1000);        
        writeInScreen("Esperando...");
    }
    if(digitalRead(BUTTON_CANCEL) == LOW){
        alert = true;
        formatedDate.toCharArray(formatedTime, 20);
        Serial.println("pulsado boton cancel");
        mesh.publish(out_topic, "CANCEL", MSG_TYPE_NONE);
        writeInScreen("CANCEL");
        delay(1000);
        writeInScreen("Esperando...");
    }    
    }
}

void writeInScreen(const char *msg){
  u8g2.begin();
  u8g2.clearBuffer();					// clear the internal memory
  u8g2.setFont(u8g2_font_ncenB10_tr);	// choose a suitable font
  u8g2.drawStr(0,23,msg);	// write something to the internal memory
  u8g2.sendBuffer();
  delay(500);
}

void callback(const char *topic, const char *msg) {
  Serial.print(topic);
  Serial.println(in_topic);
	Serial.println("************************************");
  Serial.print(topic);
  Serial.print("  ");
	Serial.println(msg);
  Serial.println("************************************");
  String msgStr = String(msg);
  msgStr.toLowerCase();
  alert = true;
  int index = msgStr.indexOf("content");
  msg_rcv = msgStr.substring(index, msgStr.length());
  msg_rcv.toCharArray(msg_rcv_send, 50);
  if(msgStr.indexOf("door")>0){
    writeInScreen("Aviso puerta, enviar?");
  }
  if(msgStr.indexOf("light")>0){
    writeInScreen("Aviso luz, enviar?");
  }
  if(msgStr.indexOf("fire")>0){
    writeInScreen("Aviso fuego, enviar?");
  }
  if(msgStr.indexOf("temperature")>0){
    writeInScreen("Aviso temperatura, enviar?");
  }
  if(msgStr.indexOf("hummidity")>0){
    writeInScreen("Aviso humedad, enviar?");
  }
  delay(1000);
}
