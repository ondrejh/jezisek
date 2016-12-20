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
#define SPEED 7
int spd = SPEED;

#define PROG_LEN 4
uint32_t progT[PROG_LEN] = {77,99,11,0};
int progS[PROG_LEN] = {7,14,16,3};

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
      digitalWrite(SERVO_PIN, LOW);
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
  static int progCnt = 0;
  

  switch (state) {
  case 0:
    digitalWrite(LED_PIN, 0);
    myservo.attach(SERVO_PIN);
    myservo.write(90);
    t = millis();
    progCnt = 0;
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
      myservo.write(90+(cosT[acnt++]/2));
      if (acnt>=TABLEN) {
        acnt=0;
        state++;    
      }
    }
    break;
  case 3:
    if ((millis()-t)>=spd) {
      t+=spd;
      myservo.write(90+(ampl/2)-cosT[acnt++]);
      if (acnt>=TABLEN) {
        acnt=0;
        state++;
        if (bcnt>=progS[progCnt]) {
          state++;
        }
      }
    }
    break;
  case 4:
    if ((millis()-t)>=spd) {
      t+=spd;
      myservo.write(90-(ampl/2)+cosT[acnt++]);
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
      myservo.write(90-(ampl/2)+(cosT[acnt++]/2));
      if (acnt>=TABLEN) {
        t=millis();
        state++;
      }
    }
    break;
  case 6:
    if (progCnt>=(PROG_LEN-1)) {
      state++;
    }
    else {
      if ((millis()-t)>=progT[progCnt]) {
        t = millis();
        progCnt++;
        acnt=0;
        bcnt=0;
        state=1;
      }
    }
    break;
  case 7:
    if ((millis()-t)>=500) {
      myservo.detach();
      state++;
    }
    break;
  case 8:
    if ((millis()-t)>=600) {
      digitalWrite(LED_PIN, 1);
      digitalWrite(SERVO_PIN, LOW);
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
  s += "<head>\r\n<meta charset='UTF-8'>\r\n<meta name='rating' content='general'>\r\n";
  s += "<meta name='viewport' content='width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no'>\r\n";
  s += "<meta http-equiv='X-UA-Compatible' content='IE=edge'>";
  s += "<style>\r\n";
  s += "*, *:before, *:after {-webkit-box-sizing: border-box; -moz-box-sizing: border-box; box-sizing: border-box;}\r\n";
  s += "html, body {margin: 0; padding: 0; background-color: #eee; color: #333; font-family: Cambria, Arial, serif; font-size: 20px; line-height: 1.5;  text-align: center; min-height: 100%;}\r\n";
  s += ".main {width: 100%; max-width: 400px; margin: 0 auto; min-height: 100%; padding: 20px;}\r\n";
  if (val==-1) {
    s += "ul {margin: 0; padding: 0; list-style: none;}\r\n";
    s += "li {margin: 0 0 20px; padding: 0; background: none; list-style: none; text-transform: uppercase; width: 100%; max-width: 400px;}\r\n";
    s += "li a {text-decoration: none; display: block; vertical-align: top; padding: 10px 30px; border: 1px solid #6b1511; text-align: center; color: #fff; border-radius: 10px; text-shadow: 0 1px 1px #000; box-shadow: 1px 1px 4px rgba(0, 0, 0, 0.5); background: #830e08; background: -moz-linear-gradient(top, rgba(204,49,43,1) 0%, rgba(131,14,8,1) 100%); background: -webkit-gradient(left top, left bottom, color-stop(0%, rgba(204,49,43,1)), color-stop(100%, rgba(131,14,8,1))); background: -webkit-linear-gradient(top, rgba(204,49,43,1) 0%, rgba(131,14,8,1) 100%); background: -o-linear-gradient(top, rgba(204,49,43,1) 0%, rgba(131,14,8,1) 100%); background: -ms-linear-gradient(top, rgba(204,49,43,1) 0%, rgba(131,14,8,1) 100%); background: linear-gradient(to bottom, rgba(204,49,43,1) 0%, rgba(131,14,8,1) 100%); filter: progid:DXImageTransform.Microsoft.gradient( startColorstr='#cc312b', endColorstr='#830e08', GradientType=0 );}\r\n";
    s += "li a:hover {background: #830e08; box-shadow: none;}\r\n</style>\r\n";
  }
  else {
    s += "h1 {font-size: 30px; font-weight: 700; line-height: 1.2; color: #000; padding: 0; margin: 50px 0 0;}\r\n</style>\r\n";
    s += "<meta http-equiv='refresh' content='2;url=index.html'>\r\n";
  }
  s += "<title>Ježíšek</title>\r\n</head>\r\n<body>\r\n";
  s += "<div class='main'>\r\n";
  
  // If we're setting the LED, print out a message saying we did
  if (val >= 0)
  {
    s += "<h1>Přepínám LEDku na ";
    s += (val)?"OFF":"ON";
    s += "</h1>\r\n";
  }
  else if (val == -2)
  { // If we're reading pins, print out those values:
    if (do_something == 0)
      s += "<h1>Ježíšek přichází !!!</h1>\r\n";
    else
      s += "<h1>Hele, počkej chvilku ...</h1>\r\n";
  }
  else if (val == -3)
  {
    if (do_something == 0)
      s += "<h1>Nastavuji nulovou polohu serva</h1>\r\n";
    else
      s += "<h1>Hele, počkej chvilku ...</h1>\r\n";
  }
  else
  {
    s += "<ul><li><a href='/led/1'>Rozsviť LEDku</a>\r\n<li><a href='/led/0'>Zhasni LEDku</a>\r\n";
    s += "<li><a href='/nuluj'>Nuluj servo</a>\r\n<li><a href='/jezisek'>JEŽÍŠEK</a></ul>\r\n";
  }
  s += "</div>\r\n</body>\r\n</html>\r\n";

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
  pinMode(SERVO_PIN, OUTPUT);
  digitalWrite(SERVO_PIN, LOW);
}

