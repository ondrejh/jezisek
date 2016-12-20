# Ježíšek

Dálkově ovládané zvonění ke stromečku. Zvoneček, servo, esp-12e (nodeMcu) ...

##Schéma (zhruba):

![Schéma](/schema/schema.png)

##Co už to umí:

NodeMcu je nastavené jako AP a tváří se jako web server.
* http://192.168.4.1 ... vypíše menu
* http://192.168.4.1/led/0 resp led/1 ... nastaví onboard ledku (test spojení)
* http://192.168.4.1/nuluj ... nastaví nulovou polohu serva
* http://192.168.4.1/jezisek ... zavrtí servem podle přednastaveného programu


##TODO:

Upravit interface aby:
* menu (odkazy) vypadalo jako tlačítka - dostatečně velká (hlavně na mobilu)
* ostatní "stránky" měli omezenou platnost (např. 3s) a vracely spátky do menu
