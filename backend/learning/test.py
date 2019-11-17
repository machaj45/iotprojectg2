
import paho.mqtt.client as mqtt # import the client1
import time
import argparse
import json
from bitstring import ConstBitStream
from d7a.alp.parser import Parser as AlpParser
from d7a.dll.parser import Parser as DllFrameParser, FrameType
from d7a.serial_modem_interface.parser import Parser as SerialParser
from d7a.system_files.system_file_ids import SystemFileIds
from d7a.system_files.system_files import SystemFiles

counter_of_messages = 0;
old_parameters = []
dataForMessage = {}
clevels =[]
prevd=[];

def subscribe_to_our_devices(client):
        client.subscribe("/d7/483638370025002f/#")
        client.subscribe("/d7/483638370041003f/#")
        client.subscribe("/d7/4836383700440045/#")

def on_message(client, userdata, message):
        #print("message received ", str(message.payload.decode("utf-8")))
        #print("message topic = ", message.topic)
	topic = message.topic
	message1 = str(message.payload.decode("utf-8"))
	hexstring = message1.strip().replace(' ', '')
	data = bytearray(hexstring.decode("hex"))
	parser = AlpParser()
	parsed_values = parser.parse(ConstBitStream(data), len(data))
	#print(parsed_values)
	parameters = parser.getData()
	xLevel = parser.getRxLevel()
        clevels.append(xLevel)
        print("",prevd,parameters)
        if prevd != parameters:
            for l in clevels:
                print("",l)
            prevd=parameters
            clevels.clear()

broker_address = "student-04.idlab.uantwerpen.be"

print("creating new instance")
client = mqtt.Client("P1") # create new instance 
client.on_message=on_message # attach function to callback
print("connecting to the broker")
client.connect(broker_address) # connect broker
client.loop_start() # start the loop
print("Subscribing to topic", "/d7/#")
subscribe_to_our_devices(client)
time.sleep(1000) # wait
client.loop_stop() # stop the loop
