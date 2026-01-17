
ESP32 Smart Shunt
=================
This is ESP32 project to log Victron Energy's (VE) Smart Shunt data
using serial port link. Data will sent to (linux) server applications
  - UDP data packets (1st development phase)
  - MQTT data packets  (2nd development phase)

Switch to use MQTT allow multiple client applications read and process
same measurement data.


Development Environments
------------------------
  - ESP32 is PlatformIO project (Windows development environment)
  - linux is native development (build using desktop linux virtual machine using
    shared folder with windows host machine). Build result application(s) will
    deploy into linux server's LXD container running "mosquitto" MQTT broker.
    LXD container run in QNAP NAS Virtualization Station. Container have shared
    folder with host NAS to save measured log data for Windows and Linux machines.


References:
-----------
- https://www.victronenergy.com/media/pg/SmartShunt/en/interfacing.html
- https://www.victronenergy.com/live/vedirect_protocol:faq
- https://www.tarthorst.net/victron-ve-direct/#:~:text=VE.Direct%20uses%204-pin%20JST%20connectors.%20The%20device%20has,will%20need%20the%204-pin%20JST%20PH%202.0%20type.
- https://www.vishay.com/docs/83725/4n25.pdf


ToDo:.....
