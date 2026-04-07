
Project Target
--------------
One time dirty spaget code to use Victron Energy's (VE) Smart Shunt
to log two battery charge state during winter storage time.
There is tiny ESP32 application to read Shunt's serial port messages
and send messages as MQTT and UDP messages via WiFi to linux application.
Linux application run on NAS storage's LXD container.

Example command to verify (all) MQTT messages:

  - mosquitto_sub  -h 192.168.1.184  -p 1883  -t "#"  -v

