#include <DHT.h>
#include <ESP8266WiFi.h>
#include <string.h>
#include <PubSubClient.h>
#include "config.h"
#include "thing.h"

#define LED_PIN 4
#define DHTPIN 16
#define DHTTYPE DHT11
#define PUB_INTERVAL 3000 // Send data every 3 seconds

DHT dht(DHTPIN, DHTTYPE);
unsigned long timer;  // Used to calculate interval  

void callback(char* topic, byte* payload, unsigned int length) {
  String payloadStr = "";
  for (int i = 0; i < length; i++) {
    payloadStr += (char)payload[i];
  }

  String itemKey = String(topic);
  itemKey.replace("/" + String(THING_ID) + "/", "");

  String logMsg = "Message arrived. ItemKey: [" + itemKey + "] Payload: " + payloadStr;
  Serial.println(logMsg);
  if (itemKey.equals("sw1")) {
    if (payloadStr.equals("0")) {
      digitalWrite(LED_PIN, LOW);
    }
    else {
      digitalWrite(LED_PIN, HIGH);
    }
  }
}

void sendDHTData() {
  float t = dht.readTemperature();
  Serial.print("Temperature: ");
  String topic = "/" + String(THING_ID) + "/" + "temp1";
  char data[10];
  dtostrf(t, 1, 2, data);
  Serial.println(data);
  if (client.connected()) {
    client.publish(topic.c_str(), data);
  }
}

void sendData() {
  if (millis() - timer < 3000) {
    return;
  }
  timer = millis();
  sendDHTData();
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  thing.setup_wifi(WIFI_SSID, WIFI_PASSWD);
  thing.setup_mqtt(MQTT_SERVER, 1883, callback);
}

void loop() {
  thing.loop();
  sendData();
}
