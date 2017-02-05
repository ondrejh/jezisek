#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>

//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiAPPSK[] = "sparkfun";

/////////////////////
// Pin Definitions //
/////////////////////
const int LED_PIN = D0; // Thing's onboard, blue LED

#define PIN 4
#define NUMLEDS 16
#define INTERVAL 40
WiFiServer server(80);

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMLEDS, PIN, NEO_GRB + NEO_KHZ800);

int unset() {
  static int cnt = 0;
  static uint32_t my_millis;
  if (cnt == 0) {
    cnt = NUMLEDS/2;
    my_millis = millis()+INTERVAL;
  }
  if ((millis()-my_millis)>=INTERVAL) {
    pixels.setPixelColor(NUMLEDS/2 - cnt,0,0,0);
    cnt--;
    pixels.setPixelColor(NUMLEDS/2 + cnt,0,0,0);
    pixels.show();
    my_millis = millis();
  }
  return cnt;
}

int set_color(uint8_t r,uint8_t g,uint8_t b) {
  static int cnt = 0;
  static uint32_t my_millis;
  if (cnt == 0) {
    cnt = NUMLEDS/2;
    my_millis = millis()+INTERVAL;
  }
  if ((millis()-my_millis)>=INTERVAL) {
    cnt--;
    pixels.setPixelColor(cnt,r,g,b);
    pixels.setPixelColor(NUMLEDS-1-cnt,r,g,b);
    pixels.show();
    my_millis = millis();
  }
  return cnt;
}

int unset_set_color(uint8_t r,uint8_t g,uint8_t b) {
  static int cnt = 0;
  static uint32_t my_millis;
  if (cnt == 0) {
    cnt = NUMLEDS;
    my_millis = millis()+INTERVAL;
  }
  if ((millis()-my_millis)>=INTERVAL) {
    cnt--;
    if (cnt>=(NUMLEDS/2)) {
      pixels.setPixelColor(cnt,0,0,0);
      pixels.setPixelColor(NUMLEDS-cnt-1,0,0,0);
      pixels.show();
    }
    else {
      pixels.setPixelColor(cnt,r,g,b);
      pixels.setPixelColor(NUMLEDS-1-cnt,r,g,b);
      pixels.show();
    }
    my_millis=millis();
  }
  return cnt;
}

void setup() 
{
  initHardware();
  setupWiFi();
  server.begin();
  pixels.begin();
}

void loop() 
{
  static int do_something = -1;
  static bool led_on = false;
  static uint32_t led_millis;
  int i;
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    
    switch (do_something) {
    case 0:
      if (unset()==0)
        do_something = -1;
      break;
    case 1:
      if (unset_set_color(255,0,0)==0)
        do_something = -1;
      break;
    case 2:
      if (unset_set_color(0,255,0)==0)
        do_something = -1;
      break;
    case 3:
      if (unset_set_color(0,0,255)==0)
        do_something = -1;
      break;
    case 4:
      if (unset_set_color(128,128,0)==0)
        do_something = -1;
      break;
    case 5:
      if (unset_set_color(128,0,128)==0)
        do_something = -1;
      break;
    case 6:
      if (unset_set_color(0,128,128)==0)
        do_something = -1;
      break;
    case 7:
      if (unset_set_color(128,128,128)==0)
        do_something = -1;
      break;
    default:
      do_something=-1;
      break;
    }

    if ((led_on==true) && ((millis() - led_millis)>100)) {
      led_on = false;
      digitalWrite(LED_PIN, 1);    
    }
    
    return;
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  //Serial.println(req);
  client.flush();

  // Match the request
  int val = -1; // We'll use 'val' to keep track of both the
                // request type (read/set) and value if set.
  if (req.indexOf("/0") != -1)
    val = 0; // Will clean it
  else if (req.indexOf("/R") != -1)
    val = 1; // Will set RED
  else if (req.indexOf("/G") != -1)
    val = 2; // Will set GREEN
  else if (req.indexOf("/B") != -1)
    val = 3; // Will set BLUE
  else if (req.indexOf("/Y") != -1)
    val = 4; // Will set YELLOW
  else if (req.indexOf("/M") != -1)
    val = 5; // Will set MAGENTA
  else if (req.indexOf("/C") != -1)
    val = 6; // Will set CYAN
  else if (req.indexOf("/W") != -1)
    val = 7;
  // Otherwise request will be invalid. We'll say as much in HTML

  // Set GPIO5 according to the request
  if (val >= 0) {
    digitalWrite(LED_PIN, 0);
    led_on = true;
    led_millis = millis();
  }

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
    s += "<meta http-equiv='refresh' content='1;url=index.html'>\r\n";
  }
  s += "<title>WowJák</title>\r\n</head>\r\n<body>\r\n";
  s += "<div class='main'>\r\n";
  
  // If we're setting the LED, print out a message saying we did
  if (val >= 0)
  {
    if (do_something>=0) {
      s += "<h1>Hele, počkej chvilku!<h1>/n";
    }
    else {
      s += "<h1>Přepínám WowJák na ";
      switch(val) {
        case 0:
          s += "Vypnuto";
          break;
        case 1:
          s += "Červená";
          break;
        case 2:
          s += "Zelená";
          break;
        case 3:
          s += "Modrá";
          break;
        case 4:
          s += "Žlutá";
          break;
        case 5:
          s += "Purpurová";
          break;
        case 6:
          s += "Azurová";
          break;
        case 7:
          s += "Bílá";
          break;
      }
      s += "</h1>\r\n";
    }
  }
  else
  {
    s += "<ul><li><a href='/R'>Červená</a>\r\n<li><a href='/G'>Zelená</a>\r\n";
    s += "<li><a href='/B'>Modrá</a>\r\n<li><a href='/Y'>Žlutá</a>\r\n";
    s += "<li><a href='/M'>Purpurová</a>\r\n<li><a href='/C'>Azurová</a>\r\n";
    s += "<li><a href='/W'>Bílá</a></ul>\r\n<li><a href='/0'>Zhasnout</a></ul>\r\n";
  }
  s += "</div>\r\n</body>\r\n</html>\r\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  //Serial.println("Client disonnected");

  if (do_something == -1) {
    if (val>=0)
      do_something=val;
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
  //Serial.begin(115200);
  //pinMode(DIGITAL_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
}

