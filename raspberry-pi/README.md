# Thingo.io Raspberry Pi Example

This example demonstrates how to connect raspberry pi to thingo.io using python.

## Hardware preparation

In this demo, we are using a LED and a DHT-11 temperature sensor as example.

Connect LED control pin to GPIO 5
Connect DHT-11 data pin to GPIO 4

The pins can also be configured in config.ini

## Software preparation

1. Install pip (if not installed already)

```
sudo apt-get install python-pip
```

2. Install Paho Mqtt library

```
pip install paho-mqtt
```

3. Follow the instructions to install AdaFruit DHT-11 driver

https://github.com/adafruit/Adafruit-Raspberry-Pi-Python-Code/tree/master/Adafruit_DHT_Driver_Python

4. Copy the build output "dhtreader.so" to this folder

5. Run the app using

```sudo python ./app.py```