
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
old_parameters = [0,0,0]
dataForMessage = {}
messages = []
message = {}
inp=0
gw = {}
gw["42373434002a0049"]=0
gw["4337313400210032"]=1
gw["433731340023003d"]=2
gw["463230390032003e"]=3

def subscribe_to_our_devices(client):
        client.subscribe("/d7/48363837002a0038/#")
        client.subscribe("/d7/483638370041003f/#")
        client.subscribe("/d7/4836383700440045/#")

def on_message(client, userdata, mess):
        print("M")
        global counter_of_messages
        global messages
        global message
        global old_parameters
        global gw
        global inp
	topic = mess.topic
	gateway =  topic.split("/")[3]
	message1 = str(mess.payload.decode("utf-8"))
	hexstring = message1.strip().replace(' ', '')
	data = bytearray(hexstring.decode("hex"))
	parser = AlpParser()
	parsed_values = parser.parse(ConstBitStream(data), len(data))
	parameters = parser.getData()
	rxLevel = parser.getRxLevel()
        counter_of_messages=counter_of_messages+1
        message[gw[gateway]]=rxLevel
        temprx=rxLevel
        tempgw=gw[gateway]
	#print(parameters[2])
	#print(old_parameters[2])
	#print(counter_of_messages)

        if (parameters[2] != old_parameters[2]): 
            print("new message")
	    #check gateways from message numer old_parameters[2]
	    lll = len(messages) - 1
	    print(lll)
	    #last_mess = messages.take(-1)
	    #last_message = messages[-1]
	    #print('last message: ')
	    #print(last_message[0])
	    #print(last_message[1])
	    #print(last_message[2])
	    #print(last_message[3])
            messages.append(message)
            message={}
            message[tempgw]=temprx
            old_parameters[2]=parameters[2]
            with open('testdatabase.json', 'w') as output_file:
                temp ={}
                temp["messages"]=messages
                json.dump(temp,output_file,indent=2,sort_keys=True)
            print("output saved")


	if (parameters[2] != counter_of_messages // 4 and counter_of_messages%4!=0):
            print(parameters[2],counter_of_messages // 4)
            print("miss match")
            print('last message: ')
            print(parameters[2])
            print(counter_of_messages)
            #print('len of message')
            #print(len(message))
            #print(message[1])
            #print(message[2])
            #print(len(message[gw[3]]))
            with open('errortestdatabase.json', 'w') as output_file:
                temp ={}
                temp["messages"]=messages
                json.dump(temp,output_file,indent=2,sort_keys=True)
            return 1


broker_address = "student-04.idlab.uantwerpen.be"

print("creating new instance")
client = mqtt.Client("P1") # create new instance 
client.on_message=on_message # attach function to callback
print("connecting to the broker")
client.connect(broker_address) # connect broker
client.loop_start() # start the loop
print("Subscribing to topic", "/d7/#")
subscribe_to_our_devices(client)
time.sleep(100000) # wait
client.loop_stop() # stop the loop
