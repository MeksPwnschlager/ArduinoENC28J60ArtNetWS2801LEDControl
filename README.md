# ArduinoENC28J60ArtNetWS2801LEDControl

An Arduino Sketch to map ArtNet Input to WS2801 LEDs

Some of the code is taken from the EtherCard library examples (below)
and this arduino artnet sender/receiver package: https://vvvv.org/contribution/artnet-arduino-set-v3.1-0

# To Use:

1. Connect your Arduino to your ENC28J60-Based Ethernet Module and WS2801 LED Strip

2. Install Libraries
    - Adafruit_WS2801: https://github.com/adafruit/Adafruit-WS2801-Library 
    - EtherCard: https://github.com/jcw/ethercard

3. Adjust the Sketch to fit your Pin and Network configuration
  (With some Microcontrollers you may also need to Adjust the Pin Configuration in the EtherCard Library)

  ### For Network Config
  
  look for this part in the code:
  
  ```arduino
  #define STATIC 1  // set to 1 to disable DHCP (adjust myip/gwip values below)

  #if STATIC
  // ethernet interface ip address
  static byte myip[] = { 10,20,32,220 };
  // gateway ip address
  static byte gwip[] = { 10,20,30,1 };
  #endif
  ```
  
  if you don't want to scan your network until you find the arduino, leave `STATIC` on 1
  and set your `gwip[]` (Gateway IP) and `myip[]` (The Arduinos Static IP / needs to fit in your subnet) to whatever fits your configuration.

  ### For LED Config
  
  look for this part in the code:
  
  ```arduino
  //Adafruit Stuff
  uint8_t dataPin  = 5;
  uint8_t clockPin = 6;
  uint8_t stripSize = 9;
  Adafruit_WS2801 strip = Adafruit_WS2801(stripSize, dataPin, clockPin);
  ```
  Set your Pin configuration to wherever you attached the WS2801 LED-Strip. `stripSize` correspondes to the number of LEDs on your strip.
  If you want to use WS2812/12b/11 (Neopixel= or APA LEDs you need to replace the Adafruit library with the right one!
  
  ### ENC28J60 + abnormal Arduino setup
  
  if you're using an arduino that doesn't work out of the box with the ethernet module,
  look for this part in the code of enc28j60.cpp in the EtherCard library
  
  ```c++
  void ENC28J60::initSPI () {
  ```
  change the PIN values somehow to fit your configuration.
  I did it like this: 
  
  ```c++
  void ENC28J60::initSPI () {

  #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
        const byte SPI_SS   = 53;
        const byte SPI_MOSI = 51;
        const byte SPI_MISO = 50;
        const byte SPI_SCK  = 52;
    #else                                     //pro micro
        const byte SPI_SS   = 10; 
        const byte SPI_MOSI = 16;
        const byte SPI_MISO = 14;
        const byte SPI_SCK  = 15;
    #endif
    
    pinMode(SPI_SS, OUTPUT);
    pinMode(SPI_MOSI, OUTPUT);
    pinMode(SPI_SCK, OUTPUT);   
    pinMode(SPI_MISO, INPUT);
    
    digitalWrite(SPI_MOSI, HIGH);
    digitalWrite(SPI_MOSI, LOW);
    digitalWrite(SPI_SCK, LOW);
    
    SPCR = bit(SPE) | bit(MSTR); // 8 MHz @ 16
    bitSet(SPSR, SPI2X);
  }
  ```
  
4. Flash the Arduino Project to your Arduino

5. Configure ArtNet Controller on another device to send Packets to the right IP

6. Use ArtNet Controller to Control Strip:

    Each LED are three consecutive Channels:
  
  - Channel 1 : R
  - Channel 2 : G
  - Channel 3 : B
