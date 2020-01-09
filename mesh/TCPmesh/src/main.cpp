//************************************************************
// this is a simple example that uses the painlessMesh library
//
// 1. sends a silly message to every node on the mesh at a random time between 1 and 5 seconds
// 2. prints anything it receives to Serial.print
//
//
//************************************************************
#include <Arduino.h>
#include "painlessMesh.h"

#define   MESH_PREFIX     "TFM_painlessMesh"
#define   MESH_PASSWORD   "TFM_painlessMesh"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

int nmes=0;
String connectionMap;
const char* nodeID ="GG";
char msg[50];


void sendMessage() {
  // node, number of message, send time, power rssi
  nmes++;
  // String msg = "Hello -> ";
  // msg += String(nmes);
  // msg += " <- from node ";
  // msg += mesh.getNodeId();
  int rssi= WiFi.RSSI();
  int timeTX = mesh.getNodeTime();
  snprintf (msg, 50, "%s, %d, %d, %d, ", nodeID, nmes, rssi, timeTX);
  mesh.sendBroadcast( msg );
  taskSendMessage.setInterval( TASK_SECOND * 8);//random( TASK_SECOND * 1, TASK_SECOND * 5 ));
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  // RX node, power rssi, RX time, message
  int timeRX = mesh.getNodeTime();
  int rssi= WiFi.RSSI();
  Serial.print(nodeID);
  Serial.print(", ");
  Serial.print(rssi);
  Serial.print(", ");
  Serial.print(timeRX);
  Serial.print(", ");
  Serial.println(msg);
  
  //Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.print(nodeId);
    Serial.print(", ");
    Serial.println("New connection");
    //Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
  connectionMap=mesh.subConnectionJson();
  char msg[connectionMap.length()];
  connectionMap.toCharArray(msg, connectionMap.length());
  Serial.printf("Connections in NEtwork = %s\n", msg);;
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void setup() {
  Serial.begin(115200);

//mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP | MESH_STATUS );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  //mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
}

void loop() {
  userScheduler.execute(); // it will run mesh scheduler as well
  mesh.update();
}
