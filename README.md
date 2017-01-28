# DAJM

## ESP8266 Setup i Arduino IDE
För att kunna köra ett program i Arduino IDE måste följande saker göras:
1. Gå till Fil->Inställningar
  * Klistra in [http://arduino.esp8266.com/stable/package_esp8266com_index.json](http://arduino.esp8266.com/stable/package_esp8266com_index.json) i Additional Boards Manager URLs.
  * Tryck OK.
2. Gå till Verktyg->Kort->Boards Manager
  * Ange "esp8266" i sökrutan
  * Tryck på "Install"
3. Gå till Sketch->Include Library->Manage Libraries...
  * Ange "esp8266" i sökrutan
  * Tryck på Adafruit ESP8266
  * Tryck på "Install"
