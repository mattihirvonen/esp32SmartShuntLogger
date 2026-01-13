
#include <WiFi.h>
#include <WiFiUdp.h>

// ---------------------------------------------------------------------------------------
// Connect to the WiFi router

void setup_wifi( const char *ssid, const char *password )
{
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
}

// ---------------------------------------------------------------------------------------
// Example: Create an instance of the HardwareSerial class for Serial 2
// HardwareSerial  SerialVE(2);

// Start Serial 2 with the defined RX and TX pins and a baud rate 
void setup_uart2( HardwareSerial &Serial2, int bitrate, int RxBufferSize, int TxBufferSize )
{
  // Define the RX and TX pins for Serial 2
  #define RXD2     16
  #define TXD2     17

  Serial2.setRxBufferSize( RxBufferSize );
  Serial2.setTxBufferSize( TxBufferSize );
  Serial2.begin( bitrate, SERIAL_8N1, RXD2, TXD2 );
  //
  Serial.println("Serial 2 started at 19200 baud rate");
}

// ---------------------------------------------------------------------------------------
// Example: Create an instance of WiFiUDP
// WiFiUDP  udp;

void setup_udp( WiFiUDP &udp, const int udpPort )
{
  udp.begin(udpPort);
}


void udp_send( WiFiUDP &udp, const char *udpAddress, int udpPort, const uint8_t *data, int count )
{
    udp.beginPacket(udpAddress, udpPort);
    udp.write( data, count );
    udp.endPacket();
}


// ---------------------------------------------------------------------------------------
// Example: Create an instance of MQTTClient 
// MQTTClient client;

/*
#include <MQTT.h>

// Note: Local domain names (e.g. "Computer.local" on OSX) are not supported
  // by Arduino. You need to set the IP address directly.

void messageReceived(String &topic, String &payload) {
    ...
}

client.begin("public.cloud.shiftr.io", net);
client.onMessage(messageReceived);
client.subscribe("/hello");
client.unsubscribe("/hello");
client.publish("/hello", "world");

*/