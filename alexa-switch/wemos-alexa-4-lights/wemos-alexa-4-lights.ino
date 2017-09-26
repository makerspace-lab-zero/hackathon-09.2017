#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <functional>
#include "switch.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"

// Change this before you flash
const char* ssid = "the_little_hedgehog";
const char* password = "substantial";

//  ! RELAYS ARE ON WHEN LOW !!

const int turnON = 0;
const int turnOFF = 1;
const int pressResponsiveness = 70; //how long to wait between press and depress of button

const String deviceNames[] = {"one", "two", "three", "four", "five"}; // change names of switches you say to Alexa
const int relayPins[] = {D0, D1, D2, D3};    // pins used for relays
int defaultSwitchState[] = {turnOFF, turnOFF, turnOFF, turnOFF};     // change any of these TO turnOFF if you want that switched on at WeMos startup
const int buttonPins[] = {D4, D5, D6, D7};   // pins used for buttons
int savedButtonstate[] = {turnOFF, turnOFF, turnOFF, turnOFF};       // make all switches off by default


// prototypes
boolean connectWifi();


void readButtons();
void partyHard();

//on/off callbacks 
void oneOn();
void oneOff();

void twoOn();
void twoOff();

void threeOn();
void threeOff();

void fourOn();
void fourOff();

void fiveOn();
void fiveOff();

boolean wifiConnected = false;

UpnpBroadcastResponder upnpBroadcastResponder;

Switch *one = NULL;
Switch *two = NULL;
Switch *three = NULL;
Switch *four = NULL;
Switch *five = NULL;

void setup()
{
  for(int i=0; i<4; i++){
    pinMode(relayPins[i], OUTPUT);
    pinMode(buttonPins[i], INPUT);
    digitalWrite(relayPins[i], defaultSwitchState[i]);
  }
  Serial.begin(9600);
   
  // Initialise wifi connection
  wifiConnected = connectWifi();
  
  if(wifiConnected){
    upnpBroadcastResponder.beginUdpMulticast();
    
    // Define your switches here. Max 14
    // Format: Alexa invocation name, local port no, on callback, off callback
    one = new Switch(deviceNames[0], 80, oneOn, oneOff);
    two = new Switch(deviceNames[1], 81, twoOn, twoOff);
    three = new Switch(deviceNames[2], 82, threeOn, threeOff);
    four = new Switch(deviceNames[3], 83, fourOn, fourOff);
    five = new Switch(deviceNames[4], 84, fiveOn, fiveOff);

    Serial.println("Adding switches upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*one);
    upnpBroadcastResponder.addDevice(*two);
    upnpBroadcastResponder.addDevice(*three);
    upnpBroadcastResponder.addDevice(*four);
    upnpBroadcastResponder.addDevice(*five);
  }
}
 
void loop()
{
	 if(wifiConnected){
      upnpBroadcastResponder.serverLoop();
      
      one->serverLoop();
      two->serverLoop();
      three->serverLoop();
      four->serverLoop();
      five->serverLoop();
	 }
   readButtons();
}

void readButtons(){
    for(int i=0; i<4; i++){
      int buttonState = digitalRead(buttonPins[i]); //0
      int relayState = digitalRead(relayPins[i]);
      
      if(buttonState == turnON){ //button pressed
//        Serial.print("Button ");
//        Serial.print(i);
//        Serial.print(": Pressed ");
        delay(pressResponsiveness);
        if(digitalRead(buttonPins[i]) == turnOFF){ //button depressed
//          Serial.println (" and de-pressed.");
          if (relayState == turnOFF) { //relay is off
            digitalWrite(relayPins[i], turnON);
            Serial.println("relay ON ...");
          } else {
            digitalWrite(relayPins[i], turnOFF);
            Serial.println("relay off ...");
          }
        } 
      } else {
        // do nothing, because nothing changed!
      }
      savedButtonstate[i] = buttonState;
    }
}

void partyHard(){
  Serial.print("ALL ON - ");
  digitalWrite(relayPins[0], turnON);
  digitalWrite(relayPins[1], turnON);
  digitalWrite(relayPins[2], turnON);
  digitalWrite(relayPins[3], turnON);
  delay(800);
  
  Serial.print("DIE - ");
  for(int die=0; die<3; die++){
    digitalWrite(relayPins[die], turnOFF);
    delay(200*die);
  }

  Serial.print("SPARK - ");
  digitalWrite(relayPins[0], turnON);
  delay(100);
  digitalWrite(relayPins[0], turnOFF);
  delay(100);
  digitalWrite(relayPins[0], turnON);
  delay(150);
  digitalWrite(relayPins[0], turnOFF);
  delay(500);
  digitalWrite(relayPins[0], turnON);
  delay(70);
  digitalWrite(relayPins[0], turnOFF);
  delay(70);
  digitalWrite(relayPins[0], turnON);
  delay(70);
  digitalWrite(relayPins[0], turnOFF);
  delay(500);
  
  Serial.print("STROBE - ");
  for(int strobe=0; strobe<12; strobe++){
    digitalWrite(relayPins[1], turnON);
    digitalWrite(relayPins[2], turnOFF);
    delay(50);
    digitalWrite(relayPins[1], turnOFF);
    digitalWrite(relayPins[2], turnON);
    delay(100);
  }

  Serial.print("ALL AROUND - ");
  for(int allround=0; allround<8; allround++){
    digitalWrite(relayPins[0], turnOFF);
    digitalWrite(relayPins[1], turnOFF);
    digitalWrite(relayPins[2], turnOFF);
    digitalWrite(relayPins[3], turnON);
    delay(200);
    digitalWrite(relayPins[0], turnOFF);
    digitalWrite(relayPins[1], turnON);
    digitalWrite(relayPins[2], turnOFF);
    digitalWrite(relayPins[3], turnOFF);
    delay(200);
    digitalWrite(relayPins[0], turnOFF);
    digitalWrite(relayPins[1], turnOFF);
    digitalWrite(relayPins[2], turnON);
    digitalWrite(relayPins[3], turnOFF);
    delay(200);
    digitalWrite(relayPins[0], turnON);
    digitalWrite(relayPins[1], turnOFF);
    digitalWrite(relayPins[2], turnOFF);
    digitalWrite(relayPins[3], turnOFF);
    delay(200);
  }
  Serial.println("PARTY OVER, GO HOME!");
}

void oneOn() {
    digitalWrite(relayPins[0], turnON);
    Serial.println("Alexa: Switch 1 ON ...");
}
void oneOff() {
    digitalWrite(relayPins[0], turnOFF);
    Serial.println("Alexa: Switch 1 off ...");
}

void twoOn() {
    digitalWrite(relayPins[1], turnON);
    Serial.println("Alexa: Switch 2 ON ...");
}
void twoOff() {
    digitalWrite(relayPins[1], turnOFF);
    Serial.println("Alexa: Switch 2 off ...");
}

void threeOn() {
    digitalWrite(relayPins[2], turnON);
    Serial.println("Alexa: Switch 3 ON ...");
}
void threeOff() {
    digitalWrite(relayPins[2], turnOFF);
    Serial.println("Alexa: Switch 3 off ...");
}

void fourOn() {
    digitalWrite(relayPins[3], turnON);
    Serial.println("Alexa: Switch 4 ON ...");
}
void fourOff() {
    digitalWrite(relayPins[3], turnOFF);
    Serial.println("Alexa: Switch 4 off ...");
}

void fiveOn() {
    Serial.println("Aww shit! get ready motherfucker! HERE IT COMES!");
    partyHard();
    Serial.println("Alexa: Switched THE PARTY ON ... LET'S GET SCHWIFTY IN HERE!");
}
void fiveOff() {
    digitalWrite(relayPins[0], turnOFF);
    digitalWrite(relayPins[1], turnOFF);
    digitalWrite(relayPins[2], turnOFF);
    digitalWrite(relayPins[3], turnOFF);
    Serial.println("Alexa: YOU PARTY POOPER!!!");
    delay(1000);
    digitalWrite(relayPins[0], turnON); //wait 1 sec and then turn one light on
}
// connect to wifi – returns true if successful or false if not
boolean connectWifi(){
  boolean state = true;
  int i = 0;
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 10){
      state = false;
      break;
    }
    i++;
  }
  
  if (state){
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("");
    Serial.println("Connection failed.");
  }
  
  return state;
}
