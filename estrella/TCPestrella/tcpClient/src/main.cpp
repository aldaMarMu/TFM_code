#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";

const char* host = "SERVER_IP";
const uint16_t port = 80;

const char* nodeID="BB";


// Ntp server to know time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
char formatedTime[20];

char msg[50];

int cnt=0;

void setup() {
  Serial.begin(115200);

  delay(1000); //This is only here to make it easier to catch the startup messages.  It isn't required
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(1000); //This is only here to make it easier to catch the startup messages.  It isn't required
  Serial.println("Setting time...");
  timeClient.begin();
  timeClient.setTimeOffset(3600);
  delay(1000);
}

void loop() {
  // Serial.print("connecting to ");
  // Serial.print(host);
  // Serial.print(':');
  // Serial.println(port);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    delay(5000);
    return;
  }

  // This will send a string to the server
  // Serial.println("sending data to server");
  timeClient.update();
  String formatedDate = timeClient.getFormattedTime();
  long rssi = WiFi.RSSI();
    formatedDate.toCharArray(formatedTime, 20);
  if (client.connected()) {
    cnt++;
    // node, number of message, send time, power rssi
    snprintf (msg, 50, "%s, %d, %6ld, %s, ", nodeID, cnt, rssi, formatedTime);
    client.println(msg);
  }

  // wait for data to be available
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      delay(60000);
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  // Serial.println("receiving from remote server");
  // not testing 'client.connected()' since we do not need to send data here
  while (client.available()) {
    char ch = static_cast<char>(client.read());
    Serial.print(ch);
  }

  // Close the connection
  Serial.println();
  // Serial.println("closing connection");
  client.stop();

  delay(2000); // execute once every 5 minutes, don't flood remote service
}
