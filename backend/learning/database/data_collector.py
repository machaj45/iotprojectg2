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
        #client.subscribe("/d7/483638370041003f/#")
        # This is device of Ruben
        #client.subscribe("/d7/4836383700440045/#")
        # This is device of Jola
        client.subscribe("/d7/48363837002a0038/#")

def on_message(client, userdata, message):
        #print("message received ", str(message.payload.decode("utf-8")))
        #print("message topic = ", message.topic)
	topic = message.topic
	gateway =  topic.split("/")[3]
	message1 = str(message.payload.decode("utf-8"))
	hexstring = message1.strip().replace(' ', '')
	data = bytearray(hexstring.decode("hex"))
	parser = AlpParser()
	parsed_values = parser.parse(ConstBitStream(data), len(data))
	parameters = parser.getData()
	rxLevel = parser.getRxLevel()
	tempGateway = gw[gateway]
	global old_parameters


	if((old_parameters == []) or (parameters != old_parameters)):
		global dataForMessages
		global messageOfValues
		if(len(messageOfValues) < 4 and len(messageOfValues) > 0):
			#print("Missing messages from gateways")
			#print(messageOfValues)
			if ('A' not in messageOfValues):
				messageOfValues['A'] = 120
			if ('B' not in messageOfValues):
				messageOfValues['B'] = 120
			if ('C' not in messageOfValues):
				messageOfValues['C'] = 120
			if ('D' not in messageOfValues):
				messageOfValues['D'] = 120
			print(messageOfValues)

		if(len(messageOfValues) == 4 and messageOfValues != 0):
			global messages
			tempMessage = []
			tempMessage.append(messageOfValues.get('A'))
			tempMessage.append(messageOfValues.get('B'))
			tempMessage.append(messageOfValues.get('C'))
			tempMessage.append(messageOfValues.get('D'))
			messages.append(tempMessage)
			with open('database_test3.json', 'w') as output_file:
				temp = {}
				temp["messages"] = messages
				json.dump(temp, output_file, indent=2, sort_keys=True)
			print("data added to database")
			print("  ")
			global move_on
			global first_time
			if (first_time == 1):
				move_on = move_on + 1
				if (move_on == 3):
					move_on = 0
					first_time = first_time + 1
					print("----- Go to another place!!")
			else:
				move_on = move_on + 1
				if(move_on == 4):
					move_on = 0
					print("----- Go to another place!!")

		#print("--------first case")
		old_parameters = []
		old_parameters = parameters
		global counter_of_messages
		counter_of_messages = counter_of_messages + 1
		message_number = 'message ' + str(counter_of_messages - 1)
		print(message_number)
		messageOfValues = {}
		messageOfValues = {tempGateway : rxLevel}
		#print(messageOfValues)
		#print(type(messageOfValues))
		#my_details = {'data1': parameters[0], 'data2' : parameters[1], 'data3' : parameters[2] }
		#print(type(my_details))
		#dataForMessage.clear()
		#dataForMessage[message_number] = [my_details]
		#dataForMessage[message_number].append({ 'gateway' : gateway, 'rxLevel' : rxLevel })
	else:
		#print("!!!!!     another case")
		message_number = 'message ' + str(counter_of_messages - 1)
		#print(message_number)
		#data = {}
		messageOfValues[tempGateway] = rxLevel
		#print(message_number)
		#print(messageOfValues)
		#print(message_number)
		#dataForMessage[message_number].append({ 'gateway' : gateway, 'rxLevel' : rxLevel })



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


