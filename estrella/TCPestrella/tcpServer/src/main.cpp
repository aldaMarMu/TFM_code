#include <Arduino.h>

/*
    This sketch demonstrates how to set up a simple HTTP-like server.
    The server will set a GPIO pin depending on the request
      http://server_ip/gpio/0 will set the GPIO2 low,
      http://server_ip/gpio/1 will set the GPIO2 high
    server_ip is the IP address of the ESP8266 module, will be
    printed to Serial when the module is connected.

    Esp8266WiFi Manual WebServer
*/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#ifndef STASSID
#define STASSID "WIFI_SSID"
#define STAPSK  "WIFI_PASSWORD"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

const char* nodeID = "AA";

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

// Ntp server to know time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
char formatedTime[20];

void setup() {
  Serial.begin(115200);
  delay(1000);
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("WiFi connected"));

  // Start the server
  server.begin();
  Serial.println(F("Server started"));

  // Print the IP address
  Serial.println(WiFi.localIP());

  delay(1000); //This is only here to make it easier to catch the startup messages.  It isn't required
  Serial.println("Setting time...");
  timeClient.begin();
  timeClient.setTimeOffset(3600);
  delay(1000);
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  // Serial.println(F("new client"));

  client.setTimeout(5000); // default is 1000
  timeClient.update();
  String formattedDate = timeClient.getFormattedTime();
  long rssi = WiFi.RSSI();
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  // Serial.println(F("request: "));
  // RX node, power rssi, RX time, message
  Serial.print(nodeID);
  Serial.print(", ");
  Serial.print(rssi);
  Serial.print(", ");
  Serial.print(formattedDate);
  Serial.print(", ");
  Serial.println(req);

  // read/ignore the rest of the request
  // do not client.flush(): it is for output only, see below
  while (client.available()) {
    // byte by byte is not very efficient
    client.read();
  }

  // Send the response to the client
  // it is OK for multiple small client.print/write,
  // because nagle algorithm will group them into one single packet
  client.println("server received msg");


  // The client will actually be *flushed* then disconnected
  // when the function returns and 'client' object is destroyed (out-of-scope)
  // flush = ensure written data are received by the other side
  // Serial.println(F("Disconnecting from client"));
}
