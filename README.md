# Nightscout-Wemos-OLED
Nightscout blood sugar display on a Wemos with an oled display

![Wemos with Integrated OLED](https://raw.githubusercontent.com/c9679/Nightscout-Wemos-OLED/master/WemosOLED.jpg)

![Wemos with seperate OLED](https://raw.githubusercontent.com/c9679/Nightscout-Wemos-OLED/master/Mini.jpg)

All credit and thanks goes to the original project by robjohnc at https://github.com/robjohnc/nightscout_wemos_epaper


This is a modified version to work on a Wemos with an oled display.


This my first project with Arduino and with the Wemos, so code can probably be tidied up or made more efficient. Sorry :)


You will require the following libraries:-

https://github.com/PaulStoffregen/Time

https://github.com/bblanchon/ArduinoJson - Older version required below 6

https://github.com/PaulStoffregen/Time


https://github.com/ThingPulse/esp8266-oled-ssd1306


The Wemos I used which I dont even know if its a real Wemos product was on ebay for £10, it includes a battery holder and oled 0.96" screen, it was from these guys https://enigma-shop.com/index.php?option=com_hikashop&ctrl=product&task=show&cid=1009&name=wemos-esp-8266-board-with-0-96-oled-and-battery



I havent tested the battery voltage measurement, I believe you need a 470k resistor between the battery and the A0 pin, I dont have
a 470k resistor and I didnt have 18650 battery lol...


Anyway hope someone finds this useful

1. Setup Arduino
2. Download libraries and drivers
    - Cant quite remember all the steps in this process, I just know I have the following lines in my Arduino preferences
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
       
       https://dl.espressif.com/dl/package_esp32_index.json
3. Change Wifi details 
4. Add your Nightscout URL and SHA fingerprint and if needed the screen pins e.g "SSD1306Wire  display(0x3c, D1, D2);"
5. Change sleeptime for the number of minutes between checks of the site default is every 5 minutes
6. Upload the sketch I have the board set to "LOLIN(WEMOS) D1 R2 & mini"

Note: On the smaller version with just the Wemos and the oled, I changed the code "SSD1306Wire  display(0x3c, D1, D2);" to "SSD1306Wire  display(0x3c, D3, D4);" and I connected as follows:-

VCC = 3v3

GND = GND

SCL = D4

SDA = D3

