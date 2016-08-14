#include "Adafruit_WS2801.h"
#include "SPI.h" // Comment out this line if using Trinket or Gemma
#ifdef __AVR_ATtiny85__
 #include <avr/power.h>
#endif

#include <EtherCard.h>
#include <IPAddress.h>

#define short_get_high_byte(x) ((HIGH_BYTE & x) >> 8)
#define short_get_low_byte(x)  (LOW_BYTE & x)
#define bytes_to_short(h,l) ( ((h << 8) & 0xff00) | (l & 0x00FF) );

#define STATIC 1  // set to 1 to disable DHCP (adjust myip/gwip values below)

#if STATIC
// ethernet interface ip address
static byte myip[] = { 10,20,32,220 };
// gateway ip address
static byte gwip[] = { 10,20,30,1 };
#endif

// ethernet mac address - must be unique on your network
static byte mymac[] = { 0x70,0x69,0x69,0x2D,0x30,0x31 };

byte Ethernet::buffer[500]; // tcp/ip send and receive buffer

// Art_Net Stuff
//customisation: Artnet SubnetID + UniverseID
//edit this with SubnetID + UniverseID you want to receive 
byte SubnetID = {0};
byte UniverseID = {0};
short select_universe= ((SubnetID*16)+UniverseID);

//customisation: edit this if you want for example read and copy only 4 or 6 channels from channel 12 or 48 or whatever.
const int number_of_channels=512; //512 for 512 channels
const int start_address=0; // 0 if you want to read from channel 1

//buffers
const int MAX_BUFFER_UDP=768;
byte buffer_channel_arduino[number_of_channels]; //buffer to store filetered DMX data

// art net parameters
unsigned int localPort = 6454;      // artnet UDP port is by default 6454
const int art_net_header_size=17;
const int max_packet_size=576;
char ArtNetHead[8]="Art-Net";
char OpHbyteReceive=0;
char OpLbyteReceive=0;
//short is_artnet_version_1=0;
//short is_artnet_version_2=0;
//short seq_artnet=0;
//short artnet_physical=0;
short incoming_universe=0;
boolean is_opcode_is_dmx=0;
boolean is_opcode_is_artpoll=0;
boolean match_artnet=1;
short Opcode=0;

//Adafruit Stuff
uint8_t dataPin  = 5;
uint8_t clockPin = 6; 
uint8_t stripSize = 9;
Adafruit_WS2801 strip = Adafruit_WS2801(stripSize, dataPin, clockPin);

//callback that prints received packets to the serial port
void udpSerialPrint(uint16_t dest_port, uint8_t src_ip[4], uint16_t src_port, const char *data, uint16_t len){
  IPAddress src(src_ip[0],src_ip[1],src_ip[2],src_ip[3]);
  
  Serial.print("dest_port: ");
  Serial.println(dest_port);
  Serial.print("src_port: ");
  Serial.println(src_port);
  
  Serial.print("src_ip: ");
  ether.printIp(src_ip);
  Serial.println("data: ");
  Serial.println(data);

  match_artnet=1;
  for (int i=0;i<7;i++) {
    //if not corresponding, this is not an artnet packet, so we stop reading
    if(char(data[i])!=ArtNetHead[i]) {
      Serial.println("Head Doesn't Match ArtNet");
      match_artnet=0;break;
    } 
  }
  if(match_artnet==1) { 
        Serial.println("Head Matches ArtNet");
        //artnet protocole revision, not really needed
        //is_artnet_version_1=packetBuffer[10]; 
        //is_artnet_version_2=packetBuffer[11];*/
      
        //sequence of data, to avoid lost packets on routeurs
        //seq_artnet=packetBuffer[12];*/
          
        //physical port of  dmx N°
        //artnet_physical=packetBuffer[13];*/
        
      //operator code enables to know wich type of message Art-Net it is
      Opcode=bytes_to_short(data[9],data[8]);
       
      //if opcode is DMX type
      if(Opcode==0x5000) {
        is_opcode_is_dmx=1;is_opcode_is_artpoll=0;
      }   
       
      //if opcode is artpoll 
      else if(Opcode==0x2000) {
         is_opcode_is_artpoll=1;is_opcode_is_dmx=0;
         //( we should normally reply to it, giving ip adress of the device)
      } 
       
      //if its DMX data we will read it now
      if(is_opcode_is_dmx=1) {
         
         //read incoming universe
         incoming_universe= bytes_to_short(data[15],data[14])
         //if it is selected universe DMX will be read
         if(incoming_universe==select_universe) {
       
          //getting data from a channel position, on a precise amount of channels, this to avoid to much operation if you need only 4 channels for example
          //channel position
          for(int i=start_address;i< number_of_channels;i++) {
             buffer_channel_arduino[i - start_address]= byte(data[i+art_net_header_size+1]);
          }
         }
      }
    }//end of sniffing

    for (int i=0; i < stripSize; i++){
      int offset = i*3;

      if(buffer_channel_arduino[offset] != 0
          || buffer_channel_arduino[offset+1] != 0
          || buffer_channel_arduino[offset+2] != 0){
        Serial.println(buffer_channel_arduino[offset]);
        Serial.println(buffer_channel_arduino[offset+1]);
        Serial.println(buffer_channel_arduino[offset+2]);
      }
      strip.setPixelColor(i, buffer_channel_arduino[offset], buffer_channel_arduino[offset+1], buffer_channel_arduino[offset+2]);
    }
  strip.show();
}

void setup(){
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
    clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
  #endif

  strip.begin();

  // Update LED contents, to start they are all 'off'
  strip.show();
 
  Serial.begin(9600);
  while (!Serial) {
    
   }
  Serial.println(F("\n[backSoon]"));

  Serial.println(ether.begin(sizeof Ethernet::buffer, mymac, 10));
  ether.staticSetup(myip, gwip);

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);

  //register udpSerialPrint() to port 1337
  ether.udpServerListenOnPort(&udpSerialPrint, 6454 );
  Serial.println(F("Listening on 6454"));
  //register udpSerialPrint() to port 42.
}

void loop(){
  //this must be called for ethercard functions to work.
  ether.packetLoop(ether.packetReceive());
} 


/*
//Processing sketch to send test UDP packets.

import hypermedia.net.*;

 UDP udp;  // define the UDP object


 void setup() {
 udp = new UDP( this, 6000 );  // create a new datagram connection on port 6000
 //udp.log( true );     // <-- printout the connection activity
 udp.listen( true );           // and wait for incoming message
 }

 void draw()
 {
 }

 void keyPressed() {
 String ip       = "192.168.0.200";  // the remote IP address
 int port        = 1337;    // the destination port

 udp.send("Greetings via UDP!", ip, port );   // the message to send

 }

 void receive( byte[] data ) {       // <-- default handler
 //void receive( byte[] data, String ip, int port ) {  // <-- extended handler

 for(int i=0; i < data.length; i++)
 print(char(data[i]));
 println();
 }
*/
