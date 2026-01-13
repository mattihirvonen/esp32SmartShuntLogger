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

// Create an instance of the HardwareSerial class for Serial 2
HardwareSerial  SerialVE(2);
WiFiUDP         udp;

typedef struct
{
    int  count;
    char data[BUFSIZE];
} serbuf_t;

void loop_UDP_example( void );
void loop_SmartShunt( void );


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

void udp_send( WiFiUDP &udp, const uint8_t *data, int count )
{
    udp.beginPacket(udpAddress, udpPort);
    udp.write( data, count );
    udp.endPacket();
}

//--------------------------------------------------------------------------------------

void setup()
{
  // Serial Monitor
  Serial.begin(115200);

  // Connect to Wi-Fi
  setup_wifi( ssid, password );

  // Start UDP
  udp.begin(udpPort);

  // Start Serial 2 with the defined RX and TX pins and a baud rate of 19200
  setup_uart2( SerialVE, VE_BAUD, BUFSIZE, BUFSIZE );
}


void loop() 
{
//loop_UDP_example();
  loop_SmartShunt();
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
    // Send UDP packet
    udp.beginPacket(udpAddress, udpPort);
    udp.write( (const uint8_t*) rxbuf.data, rxbuf.count );
    udp.endPacket();

    rxbuf.count = 0;
    time_us     = 0;
  }
}