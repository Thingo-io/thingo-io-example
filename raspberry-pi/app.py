import paho.mqtt.client as mqtt
import ConfigParser
import threading
import RPi.GPIO as GPIO
import dhtreader
import sys


config = ConfigParser.ConfigParser()
config.read("./config.ini")
thingID = config.get("thingo.io", "thingID")
switchPin = config.getint("device", "switch_pin")
dht11Pin = config.getint("device", "dht11_pin")
GPIO.setmode(GPIO.BCM)
GPIO.setup(switchPin, GPIO.OUT)
dhtreader.init()

def reportTemperature():
    try:
        t, h = dhtreader.read(11, dht11Pin)
        print("Temp = {0} *C".format(t))
        client.publish("/" + thingID + "/n1", t)
    except:
        print "Unexpected error:", sys.exc_info()[0]

    threading.Timer(5.0, reportTemperature).start()


def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe("/" + thingID + "/#")

    reportTemperature()


# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    itemKey = msg.topic.replace("/"+thingID+"/", "")
    print(itemKey + " " + str(msg.payload))
    if itemKey == "sw1":
        switchValue = int(msg.payload)
        GPIO.output(switchPin, switchValue)


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.username_pw_set(config.get("thingo.io", "thingID"), config.get("thingo.io", "thingSecret"))

client.connect(config.get("thingo.io", "server"), config.getint("thingo.io", "port"), 60)
client.loop_forever()
