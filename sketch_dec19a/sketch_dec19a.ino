#include <ESP8266WiFi.h>
#include <Servo.h>

#include "cos.h"

//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiAPPSK[] = "sparkfun";

/////////////////////
// Pin Definitions //
/////////////////////
const int LED_PIN = D0; // Thing's onboard, blue LED
const int SERVO_PIN = D2;

WiFiServer server(80);

Servo myservo;
int do_something = 0;
#define SPEED 5
int spd = SPEED;

int SetZero()
{
  static int state = 0;
  static uint32_t t;

  switch (state) {
  case 0:
    digitalWrite(LED_PIN, 0);
    myservo.attach(SERVO_PIN);
    myservo.write(90);
    t = millis();
    state++;
    break;
  case 1:
    if ((millis()-t)>=4900) {
      myservo.detach();
      state++;
    }
    break;
  case 2:
    if ((millis()-t)>=5000) {
      digitalWrite(LED_PIN, 1);
      state=0;
    }
    break;
  default:
    state=0;
    break;
  }
  
  return state;
}

int JezisekPrichazi()
{
  static int state = 0;
  static uint32_t t;
  static int acnt,bcnt;
  

  switch (state) {
  case 0:
    digitalWrite(LED_PIN, 0);
    myservo.attach(SERVO_PIN);
    myservo.write(90);
    t = millis();
    state++;
    break;
  case 1:
    if ((millis()-t)>=500) {
      t = millis();
      acnt=0;
      bcnt=0;
      state++;
    }
    break;
  case 2:
    if ((millis()-t)>=spd) {
      t+=spd;
      myservo.write(90+cosT[acnt++]);
      if (acnt>=TABLEN) {
        acnt=0;
        state++;    
      }
    }
    break;
  case 3:
    if ((millis()-t)>=spd) {
      t+=spd;
      myservo.write(90+ampl-cosT[acnt++]*2);
      if (acnt>=TABLEN) {
        acnt=0;
        state++;
        if (bcnt>=5) {
          state++;
        }
      }
    }
    break;
  case 4:
    if ((millis()-t)>=spd) {
      t+=spd;
      myservo.write(90-ampl+cosT[acnt++]*2);
      if (acnt>=TABLEN) {
        acnt=0;
        bcnt++;
        state--;
      }
    }
    break;
  case 5:
    if ((millis()-t)>=spd) {
      t+=spd;
      myservo.write(90-ampl+cosT[acnt++]);
      if (acnt>=TABLEN) {
        t=millis();
        state++;
      }
    }
    break;
  case 6:
    if ((millis()-t)>=500) {
      myservo.detach();
      state++;
    }
    break;
  case 7:
    if ((millis()-t)>=600) {
      digitalWrite(LED_PIN, 1);
      state=0;
    }
    break;
  default:
    state = 0;
    break;
  }

  return state;
}

void setup() 
{
  initHardware();
  setupWiFi();
  server.begin();
}

void loop() 
{
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    switch (do_something) {

    case 1:
      if (SetZero()==0)
        do_something=0;
      break;

    case 2:
      if (JezisekPrichazi()==0)
        do_something=0;
      break;

    default:
      do_something=0;
      break;
    }
    return;
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
  int val = -1; // We'll use 'val' to keep track of both the
                // request type (read/set) and value if set.
  if (req.indexOf("/led/0") != -1)
    val = 1; // Will write LED high
  else if (req.indexOf("/led/1") != -1)
    val = 0; // Will write LED low
  else if (req.indexOf("/jezisek") != -1)
    val = -2; // Will print pin reads
  else if (req.indexOf("/nuluj") != -1)
    val = -3;
  // Otherwise request will be invalid. We'll say as much in HTML

  // Set GPIO5 according to the request
  if (val >= 0)
    digitalWrite(LED_PIN, val);

  client.flush();

  // Prepare the response. Start with the common header:
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\n";
  // If we're setting the LED, print out a message saying we did
  if (val >= 0)
  {
    s += "LED is now ";
    s += (val)?"off":"on";
  }
  else if (val == -2)
  { // If we're reading pins, print out those values:
    s += "Jezisek prichazi !!!\n";
  }
  else if (val == -3)
  {
    s += "Nulova poloha serva\n";
  }
  else
  {
    s += "<ul><li><a href='/led/1'>LED ON</a>\n<li><a href='/led/0'>LED OFF</a>\n";
    s += "<li><a href='/nuluj'>nuluj servo</a>\n<li><a href='/jezisek'>JEZISEK</a></ul>\n";
  }
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  if (do_something == 0) {
    if (val==-3)
      do_something=1;
    else if (val==-2)
      do_something=2;
  }

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}

void setupWiFi()
{
  WiFi.mode(WIFI_AP);

  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = "ESP8266 Thing " + macID;

  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i=0; i<AP_NameString.length(); i++)
    AP_NameChar[i] = AP_NameString.charAt(i);

  WiFi.softAP(AP_NameChar, WiFiAPPSK);
}

void initHardware()
{
  Serial.begin(115200);
  //pinMode(DIGITAL_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  // Don't need to set ANALOG_PIN as input, 
  // that's all it can be.
}

