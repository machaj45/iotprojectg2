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

def subscribe_to_our_devices(client):
        # This is device of
        #client.subscribe("/d7/483638370025002f/#")
        # This is device of
        client.subscribe("/d7/483638370041003f/#")
        # This is device of
        #client.subscribe("/d7/4836383700440045/#")
        # This is device of
        #client.subscribe("/d7/48363837002a0038/#")

def on_message(client, userdata, message):
        print("message received ", str(message.payload.decode("utf-8")))
        print("message topic = ", message.topic)
	topic = message.topic
	gateway =  topic.split("/")[3]
	message1 = str(message.payload.decode("utf-8"))
	hexstring = message1.strip().replace(' ', '')
	data = bytearray(hexstring.decode("hex"))
	parser = AlpParser()
	parsed_values = parser.parse(ConstBitStream(data), len(data))
	print(parsed_values)
	parameters = parser.getData()
	rxLevel = parser.getRxLevel()
	global old_parameters


	if((old_parameters == []) or (parameters != old_parameters)):
		global dataForMessages
		with open('database.json', 'a') as json_file:
			json.dump(dataForMessage, json_file)
		print("data added to database")
	
		print("--------first case")
		old_parameters = []
		old_parameters = parameters
		global counter_of_messages
		counter_of_messages = counter_of_messages + 1
		message_number = 'message' + str(counter_of_messages)
		my_details = {'data1': parameters[0], 'data2' : parameters[1], 'data3' : parameters[2] }
		print(my_details)
		print("----------------------------")
		dataForMessage.clear()
		dataForMessage[message_number] = [my_details]
		dataForMessage[message_number].append({ 'gateway' : gateway, 'rxLevel' : rxLevel })
		print("last thing in first cache----------")
		print(dataForMessages)
		print("-----------------------")
	else:
		print("!!!!!     another case")
		print("--------------------")
		message_number = 'message' + str(counter_of_messages)
		print(message_number)
		#data = {}
		print("---------------------")
		print(dataForMessage)
		dataForMessage[message_number].append({ 'gateway' : gateway, 'rxLevel' : rxLevel })
		print("----------")
		print(dataForMessage)
		print("---before data --")
		print(data)
		print("----after data----")
	#print(message_number)
	
	#my_details = {'topic': message.topic, 'message-alp_command': message1, 'rxLevel' : rxLevel, 'data1' : parameters[0], 'data2' : parameters[1], 'data3' : parameters[2]}
	#data = {}
	#message_number = 'message' + str(counter_of_messages)
	#data[message_number] = [my_details]
	#data[message_number].append({'ddd' : 'lll', '11111' : '22222'})
	#print(data)
	


#with open('database.json', 'a') as json_file:
#		json.dump(dataForMessage, json_file)
#	print("data added to database")
	#print(data)

#data = {
#data['messages'] = []
#data['messages'].append({'name': 'Scott','website': 'stackabuse.com','from': 'Nebraska'})
#data['messages'].append({
#    'name': 'Larry',
#    'website': 'google.com',
#    'from': 'Michigan'
#})
#data['messages'].append({
#    'name': 'Tim',
#    'website': 'apple.com',
#    'from': 'Alabama'
#})

#with open('database.json', 'w') as outfile:
#	json.dump(data, outfile)

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


