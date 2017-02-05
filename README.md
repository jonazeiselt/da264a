DAJM
======


Kopplingar
------
| MicroSD Card Adapter | ESP8266    |
|----------------------|------------|
| CS                   | SDA (4)    |
| SCK                  | SCK        |
| MOSI                 | MOSI (MO)  |
| MISO                 | MISO (MI)  |
| VCC                  | 3.3 V (3V) |
| GND                  | GND        |

ESP8266 Setup i Arduino IDE
------
För att kunna köra ett program i Arduino IDE måste följande saker göras:

1. Gå till Fil->Inställningar
    - Klistra in <http://arduino.esp8266.com/stable/package_esp8266com_index.json> i Additional Boards Manager URLs.
	- Tryck OK.
2. Gå till Verktyg->Kort->Boards Manager
    - Ange "esp8266" i sökrutan
    - Tryck på "Install"
3. Gå till Sketch->Include Library->Manage Libraries...
    - Ange "esp8266" i sökrutan
    - Tryck på Adafruit ESP8266
    - Tryck på "Install"
    - I arduinofilen skrivs `#include "ESP8266.h"`
 
Ibland är ovanstående punkter inte tillräckliga för att få en kommunikation med ESP8266. Det krävs i vissa fall att man har installerat en drivrutin för att få en fungerande USB-kommunikation:

1. Gå till <http://www.silabs.com/products/mcu/pages/usbtouartbridgevcpdrivers.aspx> 
2. Hämta mjukvara (beroende på vilket OS du må ha)
3. Extrahera zip-filen och kör sedan x64-exe-filen  (om du har Windows x64)
4. Följ anvisningarna
5. Klart!
