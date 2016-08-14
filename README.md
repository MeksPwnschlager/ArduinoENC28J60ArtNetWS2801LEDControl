# ArduinoENC28J60ArtNetWS2801LEDControl

An Arduino Sketch to map ArtNet Input to WS2801 LEDs

# To Use:

Connect your Arduino to your ENC28J60-Based Ethernet Module and WS2801 LED Strip

Install Libraries
  Adafruit_WS2801: https://github.com/adafruit/Adafruit-WS2801-Library
  EtherCard: https://github.com/jcw/ethercard

Adjust the Sketch to fit your Pin and Network configuration
(With some Microcontrollers you may also need to Adjust the Pin Configuration in the EtherCard Library)

Flash the Arduino Project to your Arduino

Configure ArtNet Controller on another device to send Packets to the right IP

Use ArtNet Controller to Control Strip:

  Each LED are three consecutive Channels:
  
  - Channel 1 : R
  - Channel 2 : G
  - Channel 3 : B
