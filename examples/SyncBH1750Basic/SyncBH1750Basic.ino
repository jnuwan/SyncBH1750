#include <SyncBH1750.h>
#include <ESP8266WiFi.h>

SyncBH1750 lx = SyncBH1750();

void setup() {
  Serial.begin(9600);
  Serial.println("\nBH1750 Test");
  lx.begin(SyncBH1750::BH1750_ONETIME_HIGH_RES);
}

void loop() {
	Serial.print("Brightness level: ");
	Serial.println(lx.getLightIntensity());
	ESP.deepSleep(5e6);
  delay(200);
}
