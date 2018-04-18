#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino


//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
String URL;

#include "ThingSpeak.h"
unsigned long myChannelNumber = 174399;
const char * myWriteAPIKey = "KBHLKIJZQVH5PRTX";
WiFiClient client;

const byte numChars = 32;
char receivedChars[numChars]; // an array to store the received data
char * lista[3];
boolean newData = false;


void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);

    WiFiManager wifiManager;
    
    //reset saved settings
    //wifiManager.resetSettings();
    
    //set custom ip for portal
    //wifiManager.setAPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

    //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration
    wifiManager.autoConnect("SmartMeter","IFF240497");

    ThingSpeak.begin(client);


    
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
}


void loop() {
  
    if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
      recebe_string_via_serial();
      string_to_array();
      envia_dados();
    
  }

}



void recebe_string_via_serial() {
 static byte ndx = 0;
 char endMarker = '\n';
 char rc;
 
 // if (Serial.available() > 0) {
 while (Serial.available() > 0 && newData == false) {
 rc = Serial.read();

 if (rc != endMarker) {
 receivedChars[ndx] = rc;
 ndx++;
 if (ndx >= numChars) {
 ndx = numChars - 1;
 }
 }
 else {
 receivedChars[ndx] = '\0'; // terminate the string
 ndx = 0;
 newData = true;
 Serial.println("String recebida!");
 }
 }


}

void string_to_array(){
  int i=0;
  char * pch;
  pch = strtok (receivedChars," ,.-");  
  while (pch != NULL){  
    lista[i]=pch;   
    pch = strtok (NULL, " ,.-"); 
    i=i+1;
  }
}

void envia_dados() {
 if (newData == true) {

    ThingSpeak.setField(1,lista[0]);
    ThingSpeak.setField(2,lista[1]);
    ThingSpeak.setField(3,lista[2]);
    // Write the fields that you've set all at once.
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    Serial.println("Dados enviados!");

    newData = false;
 }
}
