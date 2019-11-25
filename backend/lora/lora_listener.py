import paho.mqtt.client as mqtt # import the client1
import time
import argparse
import json

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

# mosquitto_sub -h eu.thethings.network -t '+/devices/+/up/#' -u 'theultimateapplication'
# -P 'ttn-account-v2.gGuc6mi3k6sF6W19mIYw1cOvkMjfMLlseSOT6LPtXRw' -v


def subscribe_to_our_devices(client):
        client.subscribe("+/devices/+/up/#")

def on_message(client, message):
	print("message come")
        #print("message received ", str(message.payload.decode("utf-8")))
        #print("message topic = ", message.topic)
#	topic = message.topic
#	gateway =  topic.split("/")[3]
#	message1 = str(message.payload.decode("utf-8"))
##	hexstring = message1.strip().replace(' ', '')
#	data = bytearray(hexstring.decode("hex"))
#	parser = AlpParser()
#	parsed_values = parser.parse(ConstBitStream(data), len(data))
#	parameters = parser.getData()
#	rxLevel = parser.getRxLevel()
#	tempGateway = gw[gateway]
#	global old_parameters


#	if((old_parameters == []) or (parameters != old_parameters)):
#		global dataForMessages
#		global messageOfValues
#		if(len(messageOfValues) < 4 and len(messageOfValues) > 0):
			#print("Missing messages from gateways")
			#print(messageOfValues)
#			if ('A' not in messageOfValues):
#				messageOfValues['A'] = 120
#			if ('B' not in messageOfValues):
#				messageOfValues['B'] = 120
#			if ('C' not in messageOfValues):
#				messageOfValues['C'] = 120
#			if ('D' not in messageOfValues):
#				messageOfValues['D'] = 120
#			print(messageOfValues)

#		if(len(messageOfValues) == 4 and messageOfValues != 0):
#			global messages
#			tempMessage = []
#			tempMessage.append(messageOfValues.get('A'))
#			tempMessage.append(messageOfValues.get('B'))
#			tempMessage.append(messageOfValues.get('C'))
#			tempMessage.append(messageOfValues.get('D'))
#			messages.append(tempMessage)
#			with open('database_test3.json', 'w') as output_file:
#				temp = {}
#				temp["messages"] = messages
#				json.dump(temp, output_file, indent=2, sort_keys=True)
#			print("data added to database")
#			print("  ")
#			global move_on
#			global first_time
#			if (first_time == 1):
#				move_on = move_on + 1
#				if (move_on == 3):
#					move_on = 0
#					first_time = first_time + 1
#					print("----- Go to another place!!")
#			else:
#				move_on = move_on + 1
#				if(move_on == 4):
#					move_on = 0
#					print("----- Go to another place!!")


#		old_parameters = []
#		old_parameters = parameters
#		global counter_of_messages
#		counter_of_messages = counter_of_messages + 1
#		message_number = 'message ' + str(counter_of_messages - 1)
#		print(message_number)
#		messageOfValues = {}
#		messageOfValues = {tempGateway : rxLevel}
#	else:
#		message_number = 'message ' + str(counter_of_messages - 1)
#		messageOfValues[tempGateway] = rxLevel


broker_address = "eu.thethings.network"

def on_connect(client, userdata, flags, rc):
	if(rc==0):
		print("connected ok")

print("creating new instance")
client = mqtt.Client("P1") # create new instance
client.on_connect = on_connect
client.on_message=on_message # attach function to callback
print("connecting to the broker")
client.connect(broker_address) # connect broker
client.username_pw_set(username="theultimateapplication", password="ttn-account-v2.gGuc6mi3k6sF6W19mIYw1cOvkMjfMLlseSOT6LPtXRw")
client.loop_start() # start the loop
print("Subscribing to topic", "/d7/#")
subscribe_to_our_devices(client)
time.sleep(1000000) # wait
client.loop_stop() # stop the loop


