
#include <ESP8266SDUpdater.h>
SDUpdater sdUpdater;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("HelloWorld");
  delay(1000);
}

void loop() {
  Serial.println("Will reload the menu");
  sdUpdater.updateFromSD();
  ESP.restart();
}
