/**
   第12课 GPS解析

*/
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
TinyGPSPlus gps;
static const int RXPin = 14, TXPin = 12;//d5,d6
SoftwareSerial swSer(RXPin, TXPin, false, 256);//rx,tx,
static const uint32_t GPSBaud = 9600;


void setup() {
  swSer.begin(GPSBaud);
  Serial.begin(115200);

}

void loop() {

  while (swSer.available() > 0) {
    gps.encode(swSer.read());
    // Serial.write(swSer.read());
    if (gps.location.isUpdated()) {


      Serial.print("Latitude= ");
      Serial.print(gps.location.lat(), 6);
      Serial.print(" Longitude= ");
      Serial.println(gps.location.lng(), 6);
    } else {
      Serial.println("获取不到经纬度");
    }

  }
}
