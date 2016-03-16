#include <Bridge.h>
#include <YunClient.h>
#include <Servo.h>
#include <PubSubClient.h>

#define MQTT_SERVER "mqtt.thingo.io"
#define MQTT_SERVER_PORT 1883
#define THING_ID "f{Thing ID}}"
#define THING_SECRET "{{Thing Secret}}"

#define PUBLISH_INTERVAL 3000 // 3 seconds
#define PUBLISH_TOPIC "rotation"

YunClient yun;
PubSubClient pubsub(yun);
Servo servo;

int servoPin = 9;          // Digital pin used to connect to the servo object
unsigned long time;

void setup() {
  // We'll send debugging information via the Console since we are connecting
  // to the Arduino Yun wirelessly
  Bridge.begin();
  Console.begin();
  while (!Console);

  // Prepare connection to our servo
  servo.attach(servoPin);

  // Prepare our PubSub connection
  pubsub.setServer(MQTT_SERVER, MQTT_SERVER_PORT);
  pubsub.setCallback(callback);

  Console.println("Starting up...");
}

void loop() {
  // Keep our connection Thingo.io via MQTT established all the time
  if (!pubsub.connected()) {
    char mqttTopic[40];
    sprintf(mqttTopic, "/%s/#", THING_ID);
    connect(THING_ID, THING_SECRET, mqttTopic);
  }
  pubsub.loop();

  // Read the actual servo value and publish reading to Thingo.io within a set time interval
  if (millis() > (time + PUBLISH_INTERVAL)) {
    time = millis();
    String topic = "/" + String(THING_ID) + "/" + PUBLISH_TOPIC;
    char data[10];
    int current = servo.read();
    dtostrf(current, 1, 2, data);
    Console.print("Publish payload to Thingo.io: ");
    Console.println(data);
    if (pubsub.connected()) {
      pubsub.publish(topic.c_str(), data);
    }
  }
}

void connect(const char* user, const char* passwd, const char* topic) {
  // Loop until we're reconnected
  do {
    Console.print("Attempting MQTT connection...");
    // Attempt to connect
    if (pubsub.connect(user, user, passwd)) {
      Console.println("connected");
      pubsub.subscribe(topic);
    } else {
      Console.print("failed, rc=");
      Console.print(pubsub.state());
      Console.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000) ;
    }
  } while (!pubsub.connected());
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Read the payload
  String payloadStr = "";
  for (int i = 0; i < length; i++) {
    payloadStr += (char)payload[i];
  }
  
  // Move the servo based on payload value
  servo.write(payloadStr.toInt());
  delay(100); // Wwait for the servo to reach the position

  // Print some debugging information
  String itemKey = String(topic);
  itemKey.replace("/" + String(THING_ID) + "/", "");
  String logMsg = "Message control arrived. Item Key: [" + itemKey + "] Payload: " + payloadStr;
  Console.println(logMsg);
}