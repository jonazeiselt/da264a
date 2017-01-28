# DAJM

## ESP8266 Setup i Arduino IDE
För att kunna köra ett program i Arduino IDE måste följande saker göras:
1. Gå till Fil->Inställningar
  1. Klistra in [http://arduino.esp8266.com/stable/package_esp8266com_index.json](http://arduino.esp8266.com/stable/package_esp8266com_index.json) i Additional Boards Manager URLs.
  2. Tryck OK.
2. Gå till Verktyg->Kort->Boards Manager
  1. Ange "esp8266" i sökrutan
  2. Tryck på "Install"
3. Gå till Sketch->Include Library->Manage Libraries...
  1. Ange "esp8266" i sökrutan
  2. Tryck på Adafruit ESP8266
  3. Tryck på "Install"
