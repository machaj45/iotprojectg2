import paho.mqtt.client as mqtt # import the client1
import time
import argparse
from bitstring import ConstBitStream
from d7a.alp.parser import Parser as AlpParser
from d7a.dll.parser import Parser as DllFrameParser, FrameType
from d7a.serial_modem_interface.parser import Parser as SerialParser
from d7a.system_files.system_file_ids import SystemFileIds
from d7a.system_files.system_files import SystemFiles

def subscribe_to_our_devices(client):
        # This is device of
        client.subscribe("/d7/483638370025002f/#")
        # This is device of
        client.subscribe("/d7/483638370041003f/#")
        # This is device of
        client.subscribe("/d7/4836383700440045/#")
        # This is device of
        client.subscribe("/d7/48363837002a0038/#")

def on_message(client, userdata, message):
        print("message received ", str(message.payload.decode("utf-8")))
        print("message topic = ", message.topic)
        print("message qos = ", message.qos)
        print("message retain flag = ", message.retain)



broker_address = "student-04.idlab.uantwerpen.be"

print("creating new instance")
client = mqtt.Client("P1") # create new instance 
client.on_message=on_message # attach function to callback
subscribe_to_our_devices(client)
print("connecting to broker")
client.connect(broker_address) # connect broker
client.loop_start() # start the loop
print("Subscribing to topic", "/d7/#")


time.sleep(1000) # wait
client.loop_stop() # stop the loop


