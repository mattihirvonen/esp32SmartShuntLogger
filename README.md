
ESP32 Smart Shunt
=================
This is ESP32 project to log Victron Energy's (VE) Smart Shunt data
using serial port link. Data will sent to (linux) server applications
  - UDP data packets (1st development phase)
  - MQTT data packets  (2nd development phase)

Switch to use MQTT allow multiple client applications read and process
same measurement data


Project is implemented
  - ESP32 is PlatformIO project (Windows development environment)
  - linux is native development (build using desktop linux virtual machine using
    shared folder with windows host machine). Build result application(s) will
    deploy into linux server's LXD container running "mosquitto" MQTT broker.
    LXD container run in QNAP NAS Virtualization station. Container have shared
    folder with host NAS to save measured log data.


References:
-----------
- 


ToDo:.....
