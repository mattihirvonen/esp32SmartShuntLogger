// https://joy-it.net/en/products/SBC-NodeMCU-ESP32
// https://joy-it.net/files/files/Produkte/SBC-NodeMCU-ESP32/SBC-NodeMCU-ESP32-Manual-2021-06-29.pdf
// - Flash 4 MB
//
// Pins:
// - 1 GND    (brown)
// - 2 RX     (green)
// - 3 TX     (white)
// - 4 Power+ (yellow)
//
// https://www.victronenergy.com/media/pg/SmartShunt/en/interfacing.html
// https://www.victronenergy.com/live/vedirect_protocol:faq
// https://www.tarthorst.net/victron-ve-direct/#:~:text=VE.Direct%20uses%204-pin%20JST%20connectors.%20The%20device%20has,will%20need%20the%204-pin%20JST%20PH%202.0%20type.
// https://www.vishay.com/docs/83725/4n25.pdf

#include <WiFi.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>     // MQTT
//#include <MQTT.h>

// BLE, MQTT, ...
// https://dashio.io/guide-arduino-esp32/

#include "esp32lib.hpp"

// Wi-Fi credentials - Set #if to zero when use local defines in this source
#if 1
#include "WiFiConf.h"   // Hide my secrets here !!!
#else
const char* ssid     = "YOUR_ROUTER_WiFi_SSID";
const char* password = "YOUR_ROUTER_WiFi_PASSWORD";
#endif

// UDP settings
const char* udpAddress = "192.168.1.184";  // Receiver IP
const int   udpPort    =  8080;            // Receiver Port

#define VE_BAUD  19200  // Victron Energy Smart Shunt
#define BUFSIZE  256

// Create an instance of the HardwareSerial class for Serial 2 and UDP
HardwareSerial  SerialVE(2);
WiFiUDP         udp;
//MQTTClient      client;

typedef struct
{
    int  count;
    char data[BUFSIZE];
} serbuf_t;

void loop_UDP_example( void );
void loop_SmartShunt( void );

//--------------------------------------------------------------------------------------
// Replace with your MQTT broker details
const char*  mqtt_server = "192.168.1.184";  // "broker.hivemq.com";

WiFiClient   espClient;
PubSubClient mqttClient( espClient );

void mqtt_callback(char* topic, byte* message, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  String msg;
  for (int i = 0; i < length; i++) {
    msg += (char)message[i];
  }
  Serial.println("Message: " + msg);
}

//--------------------------------------------------------------------------------------

void setup()
{
  // Serial Monitor
  Serial.begin(115200);

  // Connect to Wi-Fi
  setup_wifi( ssid, password );

  // Start UDP
  setup_udp( udp, udpPort );

  // Start Serial 2 with the defined RX and TX pins and a baud rate of 19200
  setup_uart2( SerialVE, VE_BAUD, BUFSIZE, BUFSIZE );

  //client.setServer(udpAddress, WiFi);
  setup_mqtt( mqttClient, mqtt_server, 1883 );
  mqttClient.setCallback( mqtt_callback );

}


void loop() 
{
  //loop_UDP_example();
  loop_SmartShunt();

  if (!mqttClient.connected()) {
    mqtt_reconnect( mqttClient );
  }
  mqttClient.loop();

  // Publish a message every 5 seconds
  static unsigned long lastMsg = millis();
  if (millis() - lastMsg > 5000) {
    lastMsg = millis();
    String message = "Hello from ESP32!";
    mqttClient.publish("test/topic", message.c_str(), message.length()+1);
    Serial.println("Message published: " + message);
  }
}

//------------------------------------------------------------------

void loop_UDP_example( void )
{
  // Message to send
  String message = "Hello from ESP32!";

  // Send UDP packet
  udp.beginPacket(udpAddress, udpPort);
  udp.print(message);
  udp.endPacket();

  Serial.println("UDP packet sent: " + message);

  delay(2000); // Send every 2 seconds
}


void loop_SmartShunt( void )
{
  #define  MSG_TIMEOUT_us  100000   //  Smart Shunt message period is one second

  static int  init    = 0;
  static int  time_us = 0;

  static serbuf_t txbuf;
  static serbuf_t rxbuf;

  if ( !init ) {
    Serial.print("Loop  running on core ");
    Serial.print(xPortGetCoreID());
    Serial.print(", priority ");
    Serial.println( uxTaskPriorityGet(NULL) );
    init = 1;
  }

  while (Serial.available() > 0)
  {
    // get the byte data from the terminal
    char serData = Serial.read();
//  SerialVE.print(serData);

    strcpy( txbuf.data, "Heippa!");
    txbuf.count = strlen( txbuf.data );

    int count = SerialVE.write(txbuf.data, txbuf.count);
  }

  while (SerialVE.available() > 0)
  {
    time_us = esp_timer_get_time();  // uint64_t esp_timer_get_time();

    // get the byte data from the VE Smart Shunt
    char serData = SerialVE.read();

    if ( rxbuf.count < BUFSIZE ) {
      rxbuf.data[ rxbuf.count++ ] = serData;
    }
  }

  int  time_now = esp_timer_get_time();

  if ( time_us && ((time_now - time_us) > MSG_TIMEOUT_us) )
  {
    udp_send( udp, udpAddress, udpPort, (const uint8_t*) rxbuf.data, rxbuf.count );

    mqttClient.publish( "smartshunt", (const uint8_t*) rxbuf.data, rxbuf.count );

    rxbuf.count = 0;
    time_us     = 0;
  }
}