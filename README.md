# Ježíšek

Dálkově ovládané zvonění ke stromečku. Zvoneček, servo, esp-12e (nodeMcu) ...
Založeno na https://learn.sparkfun.com/tutorials/esp8266-thing-hookup-guide/example-sketch-ap-web-server .. dík www.sparkfun.com za skvělý example ;-)

##Prototyp:

![Prototyp zepredu](/schema/prototyp_zepredu.jpg)

a zezadu: https://www.github.com/ondrejh/jezisek/schema/prototyp_zezadu.jpg

##Interface:

![Vzhled webového rozhraní](/schema/interface.jpg)

##Schéma (zhruba):

![Schéma](/schema/schema.png)

##Co už to umí:

NodeMcu je nastavené jako AP a tváří se jako web server.
Jméno sítě je "ESP8266 thing XXXX" a heslo "sparkfun" ...

* http://192.168.4.1 ... vypíše menu
* http://192.168.4.1/led/0 resp led/1 ... nastaví onboard ledku (test spojení)
* http://192.168.4.1/nuluj ... nastaví nulovou polohu serva
* http://192.168.4.1/jezisek ... zavrtí servem podle přednastaveného programu
