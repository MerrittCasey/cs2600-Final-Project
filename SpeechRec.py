# Author: Casey Merritt
# Class:  CS 2600
# Date:   05/20/2021

import speech_recognition as sr
from datetime import datetime
from datetime import date

import paho.mqtt.client as mqttclient
import time


r = sr.Recognizer()

connected = False
broker_address = "192.168.0.249"
port = 1883
user = "mqtt"
password = "cs2600"

def on_connect(client, usedata, flags, rc):
    if rc == 0:
        print("client is connected")
        global connect
        connected = True
    else:
        print("connection failed")

client = mqttclient.Client("MQTT")
client.username_pw_set(user, password=password)
client.on_connect = on_connect
client.connect(broker_address, port=port)
client.publish("cs2600/final", "Speech Rocognizer Connected")

print("")
print("-- Welcome to Voice Recognizer! --")
print("Say -commands- to List Available Commands")
print("")

con = True

while con:
    with sr.Microphone() as source:

        print('Say Anything: ')
        audio = r.listen(source)

        try:
            text = r.recognize_google(audio)
        except:
            text = ""

        if "quit" in text:
            print('You said: {}'.format(text))
            print("")
            print("Goodbye!")
            client.publish("cs2600/final", "Speech Rcognizer Disconnecting")
            client.disconnect()
            con = False
        elif "commands" in text:
            print('You said: {}'.format(text))
            print("")
            print("-- Command List --")
            print("Say -what time is it- For The Current System Time!")
            print("Say -what is today's date- For the Current Date!")
            print("Say -quit- To Close The Program!")
            print("-- Make Sure To Speak Clearly! --")
            print("")
        elif "what time is it" in text:
            print('You said: {}'.format(text))
            print("")
            now = datetime.now()
            current_time = now.strftime("%I:%M")
            print("The Current Time Is: ", current_time)
            command = "3 " + current_time
            client.publish("cs2600/final", command)
            print("")
        elif "what is today's date" in text:
            print('You said: {}'.format(text))
            print("")
            today = date.today()
            d = today.strftime("%b %d, %Y")
            print("Today's Date Is: ", d)
            command = "4 " + d
            client.publish("cs2600/final", command)
            print("")
        elif "on" in text:
            print('You said: {}'.format(text))
            print("")
            print("Turning LED On")
            print("")
            client.publish("cs2600/final", "1")
        elif "off" in text:
            print('You said: {}'.format(text))
            print("")
            print("Turning LED Off")
            print("")
            client.publish("cs2600/final", "2")
        else:
            print('You said: {}'.format(text))
            print("")
            print("Sorry command not recognized, Try Again!")
            print("")
