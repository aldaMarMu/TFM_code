#define NETWORK_PASSWORD        "WIFI_PASSWORD"
#define NETWORK_LIST { \
	WIFI_CONN("WIFI_SSID", NETWORK_PASSWORD, NULL, 0), \
	WIFI_CONN("WIFI_SSID", NETWORK_PASSWORD, NULL, 0), \
        NULL, \
        }
#define MESH_PASSWORD    "TFM_mesh"
#define MQTT_SERVER      "SERVER_IP"
#define MQTT_PORT        1883

/* Only used if SSL is enabled */
#define MESH_SECURE      false
#define MQTT_SECURE      false
#define MQTT_FINGERPRINT NULL
//const uint8_t MQTT_FINGERPRINT[] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff,0x00,0x11,0x22,0x33};
#define ASYNC_TCP_SSL_ENABLED false
