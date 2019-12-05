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

first_time = 1
move_on = 0
counter_of_messages = 0
old_parameters = []
dataForMessage = {}
gw = {}
messageOfValues = {}
messages = []
gw["42373434002a0049"] = 'A'
gw["4337313400210032"] = 'B'
gw["433731340023003d"] = 'C'
gw["463230390032003e"] = 'D'

#print(len(datas.A))

def subscribe_to_our_devices(client):
        # This is device of Hanna
        #client.subscribe("/d7/483638370025002f/#")
        # This is device of Jan
        client.subscribe("/d7/483638370041003f/#")
        # This is device of Ruben
        client.subscribe("/d7/4836383700440045/#")
        # This is device of Jola
        client.subscribe("/d7/48363837002a0038/#")

def on_message(client, userdata, message):
	print("on message")
        #print("message received ", str(message.payload.decode("utf-8")))
        #print("message topic = ", message.topic)
	topic = message.topic
	gateway =  topic.split("/")[3]
	message1 = str(message.payload.decode("utf-8"))
	hexstring = message1.strip().replace(' ', '')
	data = bytearray(hexstring.decode("hex"))
	parser = AlpParser()
	parsed_values = parser.parse(ConstBitStream(data), len(data))
	#print("-------")
	#print(gw[gateway])
	#print(parsed_values)
	#print("-------")
	parameters = parser.getData()
	rxLevel = parser.getRxLevel()
	tempGateway = gw[gateway]
	print(tempGateway)
	print(rxLevel)
	#my_details = {'topic': message.topic, 'message-alp_command': message1, 'rxLevel' : rxLevel, 'data1' : parameters[0], 'data2' : parameters[1], 'data3' : parameters[2]}
	#data = {}
	#message_number = 'message' + str(counter_of_messages)
	#data[message_number] = [my_details]
	#data[message_number].append({'ddd' : 'lll', '11111' : '22222'})


broker_address = "student-04.idlab.uantwerpen.be"

print("creating new instance")
client = mqtt.Client("P1") # create new instance 
client.on_message=on_message # attach function to callback
print("connecting to the broker")
client.connect(broker_address) # connect broker
client.loop_start() # start the loop
print("Subscribing to topic", "/d7/#")
subscribe_to_our_devices(client)
time.sleep(1000000) # wait
client.loop_stop() # stop the loop


