
void setup_wifi( const char *ssid, const char *password );

void setup_uart2( HardwareSerial &Serial2, int bitrate, int RxBufferSize, int TxBufferSize );

void setup_udp( WiFiUDP &udp, const int udpPort );

void udp_send( WiFiUDP &udp, const char *udpAddress, int udpPort, const uint8_t *data, int count );
