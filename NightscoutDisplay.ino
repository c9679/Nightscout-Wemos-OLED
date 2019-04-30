#include <OLEDDisplay.h>
#include <OLEDDisplayFonts.h>
#include <OLEDDisplayUi.h>
#include <SSD1306Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h> // have to use version 5.
#include <TimeLib.h>
#include <Time.h>

SSD1306Wire display(0x3c, D1, D2);

const char* ssid = "wifissid"; //enter your SSID
const char* password = "password"; //enter your wifi password
//Miutes to sleep for
const int sleepTimeS = 5;

// this section gets the current voltage (approximately) on pin A0 if it is connected in a resistor voltage divider like this: VCC -> 100K -> A0 -> 330K -> GND
int raw = analogRead(A0);
float volt=raw/1023.0;
float getVcc=raw / 194.2;//*4.408;

const char* host = "www.yoursite.com"; //enter your nightscout host here - no "https://" needed
const int httpsPort = 443;
  const size_t bufferSize = 3*JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + 2*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(13) + 280;
// SHA1 fingerprint of the host's SSL certificate
// Goto https://www.grc.com/fingerprints.htm type in ur sites URL to obtain the fingerprint
const char* fingerprint = "08:3B..."; //enter the https fingerprint if you want this verification


const unsigned char ArrowUp [] PROGMEM = {
0x80, 0x00, 0xc0, 0x01, 0xe0, 0x03, 0xf0, 0x07, 0xf8, 0x0f, 0xfc, 0x1f, 0xde, 0x3d, 0xcf, 0x79, 0xc7, 0x71, 0xc0, 0x01, 0xc0, 0x01, 0xc0, 0x01, 0xc0, 0x01, 0xc0, 0x01, 0xc0, 0x01, 0xc0, 0x01,
};
const unsigned char ArrowDown [] PROGMEM = {
0xc0, 0x01, 0xc0, 0x01, 0xc0, 0x01, 0xc0, 0x01, 0xc0, 0x01, 0xc0, 0x01, 0xc0, 0x01, 0xc7, 0x71, 0xcf, 0x79, 0xde, 0x3d, 0xfc, 0x1f, 0xf8, 0x0f, 0xf0, 0x07, 0xe0, 0x03, 0xc0, 0x01, 0x80, 0x00,
};
const unsigned char ArrowUpS [] PROGMEM = {
0x00, 0x00, 0x00, 0x00, 0xf8, 0x3f, 0xf8, 0x3f, 0xf8, 0x3f, 0x00, 0x3f, 0x80, 0x3f, 0xc0, 0x3f, 0xe0, 0x3f, 0xf0, 0x39, 0xf8, 0x38, 0x7c, 0x38, 0x3c, 0x38, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00,
};
const unsigned char ArrowDownS [] PROGMEM = {
0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x3c, 0x38, 0x7c, 0x38, 0xf8, 0x38, 0xf0, 0x39, 0xe0, 0x3f, 0xc0, 0x3f, 0x80, 0x3f, 0x00, 0x3f, 0xf8, 0x3f, 0xf8, 0x3f, 0xf8, 0x3f, 0x00, 0x00, 0x00, 0x00,
};
const unsigned char ArrowSide [] PROGMEM = {
0x80, 0x01, 0x80, 0x03, 0x80, 0x07, 0x00, 0x0f, 0x00, 0x1e, 0x00, 0x3c, 0xff, 0x7f, 0xff, 0xff, 0xff, 0x7f, 0x00, 0x3c, 0x00, 0x1e, 0x00, 0x0f, 0x80, 0x07, 0x80, 0x03, 0x80, 0x01,
};

void showdata(String timenow, String BG, int age, long rssiCalc, String bgs0_direction, float volts)
{
   display.clear();
   display.setFont(ArialMT_Plain_10);
   display.setTextAlignment(TEXT_ALIGN_LEFT);
   display.drawString(2, 3, "Time: " + timenow);
   display.setTextAlignment(TEXT_ALIGN_RIGHT);
   display.drawString(126, 3, "Age: " + String(age) + " Mins");
   display.setFont(ArialMT_Plain_16);
   display.setTextAlignment(TEXT_ALIGN_CENTER);
   display.drawString(45, 18, "BG");
   display.drawString(45, 32, BG);
   if (String(bgs0_direction) == "Flat") {
     //  directarr = "→";
       display.drawXbm(101, 26, 16, 16,  ArrowSide);
    }  else if (String(bgs0_direction) == "FortyFiveUp") {
      //  directarr = "↗";
        display.drawXbm(101, 26, 16, 16,  ArrowUpS);
    }  else if (String(bgs0_direction) == "FortyFiveDown") {
       // directarr = "↘";
        display.drawXbm(101, 26, 16, 16,  ArrowDownS);
    } else if (String(bgs0_direction) == "DoubleUp") {
       //directarr = "↑↑";
       display.drawXbm(101, 26, 16, 16,  ArrowUp);
    } else if (String(bgs0_direction) == "DoubleDown") {
        //directarr = "↓↓";
        display.drawXbm(101, 26, 16, 16,  ArrowDown);
    }

      
   display.setFont(ArialMT_Plain_10);
   display.setTextAlignment(TEXT_ALIGN_LEFT);
   display.drawString(2, 50, "Batt: " + String(volts) + "v");
   display.setTextAlignment(TEXT_ALIGN_RIGHT);
   display.drawString(126, 50, "WIFI: " + String(rssiCalc) + "%");
   display.drawRect(0, 0, 128, 64);
   display.drawRect(0, 17, 128, 33);
   display.drawVerticalLine(90, 20, 27);
   display.display();

}

bool isBST(int year, int month, int day, int hour)
{
    // bst begins at 01:00 gmt on the last sunday of march
    // and ends at 01:00 gmt (02:00 bst) on the last sunday of october
    // january, february, and november are out
    if (month < 3 || month > 10) { return false; }
    // april to september are in
    if (month > 3 && month < 10) { return true; }
    // last sunday of march
    int lastMarSunday =  (31 - (5* year /4 + 4) % 7);
    // last sunday of october
    int lastOctSunday = (31 - (5 * year /4 + 1) % 7);
    // in march we are bst if its past 1am gmt on the last sunday in the month
    if (month == 3)
    {
        if (day > lastMarSunday)
        {
            return true;
        }
        if (day < lastMarSunday)
        {
            return false;
        }
        if (hour < 1)
        {
            return false;
        }
        return true;
    }
    // in october we must be before 1am gmt (2am bst) on the last sunday to be bst
    if (month == 10)
    {
        if (day < lastOctSunday)
        {
            return true;
        }
        if (day > lastOctSunday)
        {
            return false;
        }
        if (hour >= 1)
        {
            return false;
        }
        return true;
    }
}

void getreadings()
{

  // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);

  Serial.printf("Using fingerprint '%s'\n", fingerprint);
  client.setFingerprint(fingerprint);

  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 26, "Connecting to NightScout");
  display.display();
  
 while (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    delay(60000);
  }

 if (client.verify(fingerprint, host)) {
   Serial.println("certificate matches");
 
 } else {
   Serial.println("certificate doesn't match");
 
 }

  String url = "/pebble";
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  //Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      //Serial.println("headers received");
      break;
    }
  }
  String line2 = client.readStringUntil('\n');
  Serial.println("==========");
  //read the JSON line into "line2"
  line2 = client.readStringUntil('\n');
        //parse the JSON into variables
        
       DynamicJsonBuffer jsonBuffer(bufferSize);
       JsonObject& root = jsonBuffer.parseObject(line2);
        
        String status0_now = root["status"][0]["now"]; 
        status0_now = status0_now.substring(0, status0_now.length() - 3);
        time_t status0_now1 = status0_now.toInt();
        JsonObject& bgs0 = root["bgs"][0];
      
        const char* bgs0_sgv = bgs0["sgv"]; 
        int bgs0_trend = bgs0["trend"]; 
        const char* bgs0_direction = bgs0["direction"]; 
            time_t bgs0_datetime = bgs0["datetime"];
        String bgs0_datetime2 = bgs0["datetime"]; 
            bgs0_datetime2 = bgs0_datetime2.substring(0, bgs0_datetime2.length() - 3);
            time_t bgs0_datetime3 = bgs0_datetime2.toInt();
//check for British summer time and adjust display time accordingly
            if (isBST(year(status0_now1), month(status0_now1), day(status0_now1), hour(status0_now1))){
              status0_now1 = status0_now1 +3600;
              }
            if (isBST(year(bgs0_datetime3), month(bgs0_datetime3), day(bgs0_datetime3), hour(bgs0_datetime3))){
              bgs0_datetime3 = bgs0_datetime3 +3600;
              }
            

        const char* bgs0_battery = bgs0["battery"]; 
        JsonObject& cals0 = root["cals"][0];
        float cals0_slope = cals0["slope"]; 
        float cals0_intercept = cals0["intercept"]; 
        int cals0_scale = cals0["scale"]; 

        
Serial.print("Time Now: ");
Serial.print(hour(status0_now1));
Serial.print(":");
Serial.println(minute(status0_now1));
String timeNow = String(hour(status0_now1)) + ":";
if (minute(status0_now1) < 10){timeNow = timeNow + "0";} 
timeNow = timeNow + String(minute(status0_now1));
//print current time
Serial.print("As at: ");
Serial.print(hour(bgs0_datetime3));
Serial.print(":");
Serial.println(minute(bgs0_datetime3));
int dataAge = (status0_now1 - bgs0_datetime3) / 60;
Serial.print("Data Age (minutes): ");
Serial.println(dataAge);
Serial.print("Blood Glucose is:  ");
Serial.print(String(bgs0_sgv) + " ");
char* directarr = "";

Serial.println(directarr);

Serial.println("Battery Voltage is:  ");
Serial.println(getVcc);

long rssi = WiFi.RSSI();
long rssiCalc;
if(rssi <= -100)
        rssiCalc = 0;
    else if(rssi >= -50)
        rssiCalc = 100;
    else
rssiCalc = 2 * (rssi + 100);

  //call the "showdata" function to show the data on the LCD screen
  showdata(timeNow,bgs0_sgv,dataAge,rssiCalc,bgs0_direction,getVcc);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);
             digitalWrite(LED_BUILTIN, LOW);         
  Serial.begin(115200);
  Serial.setTimeout(2000);
  Serial.println();
  Serial.println(raw);
  Serial.println(getVcc);

  display.init();
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 26, "Connecting to WIFI");
  display.drawString(64, 40, ssid);
  display.display();

 
  Serial.println("Connecting to WIFI");
  
 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected to WIFI");
  Serial.println(WiFi.localIP());
  
  getreadings();
  
  //sleep for n minutes
  
  delay(sleepTimeS * 60000);
}
void loop()
{
  pinMode(LED_BUILTIN, OUTPUT);
  //digitalWrite(LED_BUILTIN, HIGH); 
  getreadings();
  //digitalWrite(LED_BUILTIN, LOW); 
  delay(300000);
}
