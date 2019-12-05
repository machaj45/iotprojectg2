import paho.mqtt.client as mqtt # import the client1
import time
import ttn
import argparse
import json
import struct
import sys
import datetime
import math
sys.path.insert(1, '../learning/knn')
from KNN import *
from bitstring import ConstBitStream
from d7a.alp.parser import Parser as AlpParser
from d7a.dll.parser import Parser as DllFrameParser, FrameType
from d7a.serial_modem_interface.parser import Parser as SerialParser
from d7a.system_files.system_file_ids import SystemFileIds
from d7a.system_files.system_files import SystemFiles

class mainservice:

    def __init__(self):
	self.counter_of_messages = 0;
	self.old_index = -1
    	self.dataForMessage = {}
    	self.messageOfValues = {}
    	self.A = "42373434002a0049"
    	self.B = "4337313400210032"
    	self.C = "433731340023003d"
    	self.D = "463230390032003e"
    	self.gw = {}
    	self.gw[self.A] = 'A'
    	self.gw[self.B] = 'B'
    	self.gw[self.C] = 'C'
    	self.gw[self.D] = 'D'
    	self.client = mqtt.Client("P1") # create new instance
    	self.client_pub = mqtt.Client("P2")
    	self.broker_address = "student-04.idlab.uantwerpen.be"
	self.app_id_lora = "theultimateapplication"
	self.access_key_lora = "ttn-account-v2.gGuc6mi3k6sF6W19mIYw1cOvkMjfMLlseSOT6LPtXRw"
	self.kn = KNN('../learning/knn/database.json', '../learning/knn/points.json')
	self.parameters_to_send_with_dash7 = [0, 0, 0]

        print("Starting program")
        self.client.on_message=self.on_message # attach function to callback
        print("Connecting to the broker DASH7")
        self.client.connect(self.broker_address) # connect broker
        self.client.loop_start() # start the loop # loop_forever()
        print("Subscribing to topic", "/d7/#")
        self.subscribe_to_our_devices()

	#lora
	self.handler = ttn.HandlerClient(self.app_id_lora, self.access_key_lora)
	self.mqtt_client_lora = self.handler.data()
	self.mqtt_client_lora.set_uplink_callback(self.uplink_callback)
	self.mqtt_client_lora.connect()
	print("Connecting to the broker Lora")

	time.sleep(1000) # wait
        self.client.loop_stop() # stop the loop for dash7 listening
	self.mqtt_client_lora.close()

    def subscribe_to_our_devices(self):
        # This is device of Jan
        self.client.subscribe("/d7/483638370041003f/#")
        # This is device of Ruben
        self.client.subscribe("/d7/4836383700440045/#")
        # This is device of Jola
        self.client.subscribe("/d7/48363837002a0038/#")

    def uplink_callback(self,msg, client):
        print("Uplink started")
        #print("Received uplink from ", msg.dev_id)
	temp_lora = msg.payload_fields.temp
	humi_lora = msg.payload_fields.humid
	CO2_lora = msg.payload_fields.CO2
        #print("counter: ", msg.counter)
	data_from_lora = [temp_lora, humi_lora, CO2_lora]
	self.publish_info_to_things_board(data_from_lora)

#    def on_connect(client, userdata, flags, rc):
#	print("Connection returned result: " + connack_string(rc))

#    def on_publish(client, userdata, result):
#	print("data published")

#    def on_disconnect(client, userdata, rc):
#	print("client disconnected ok")

    def publish_info_to_things_board(self, data):
	#access_token = 'wFRAZQnB2t8hIol30gkm' #Jola
	access_token = 'IWjQcWm3Q2PEiY9duRQP' #Jan
	#access_token = 'P6S9p9cWsikzbqRNgQ7f' #Ruben
	broker = "thingsboard.idlab.uantwerpen.be"
	topic = "v1/devices/me/telemetry"
	#self.client_pub.on_connect = on_connect
	#self.client_pub.on_publish = on_publish
	self.client_pub.username_pw_set(access_token)
	self.client_pub.connect(broker, keepalive=20)
	message_to_send = {"temperature" : data[0], "humidity" : data[1], "CO2" : data[2]}
	result = json.dumps(message_to_send)
	self.client_pub.publish(topic, result)
	print("Data = ", message_to_send)
	print("Data published successfully to ThingsBoard!")
        #self.client_pub.on_disconnect = on_disconnect
        self.client_pub.disconnect()
	print("Client (ThingsBoard) disconnected")

    def fingerprinting(self, rx_levels):
        tempMessage = []
        tempMessage.append(rx_levels.get('A'))
        tempMessage.append(rx_levels.get('B'))
        tempMessage.append(rx_levels.get('C'))
        tempMessage.append(rx_levels.get('D'))
	point = self.kn.getpoint([tempMessage])
	print(point)
	self.kn.plotmap(point)
	print("Fingerprinting done correctly")

    def on_message(self,client, userdata, message):
        print(" ")
	#time.sleep(10)
	print("on message")
	topic = message.topic
        gateway =  topic.split("/")[3]
        message1 = str(message.payload.decode("utf-8"))
	hexstring = message1.strip().replace(' ', '')
	data = bytearray(hexstring.decode("hex"))
        parser = AlpParser()
        parsed_values = parser.parse(ConstBitStream(data), len(data))
	parameters = parser.getData()
        rxLevel = parser.getRxLevel()
	tempGateway = self.gw[gateway]
	#print(tempGateway, rxLevel)
	#print("old one", self.old_index)
	#print("new one ", parameters[12])



	if((self.old_index < 0) or (parameters[12] != self.old_index)):
            	if(len(self.messageOfValues) < 4 and len(self.messageOfValues) > 0):
                        print("Missing messages from gateways")
			message_number = 'message' + str(self.counter_of_messages)
                        if ('A' not in self.messageOfValues):
                                self.messageOfValues['A'] = 120
				self.dataForMessage[message_number].append({ 'gateway' : self.A, 'rxLevel' : 120 })
                        if ('B' not in self.messageOfValues):
				self.messageOfValues['B'] = 120
				self.dataForMessage[message_number].append({ 'gateway' : self.B, 'rxLevel' : 120 })
                        if ('C' not in self.messageOfValues):
                                self.messageOfValues['C'] = 120
				self.dataForMessage[message_number].append({ 'gateway' : self.C, 'rxLevel' : 120})
                        if ('D' not in self.messageOfValues):
                                self.messageOfValues['D'] = 120
				self.dataForMessage[message_number].append({ 'gateway' : self.D, 'rxLevel' : 120 })

		print(self.messageOfValues)

		if (len(self.messageOfValues) == 4 and self.messageOfValues != 0):
			with open('measurements.json', 'a') as json_file:
                		json.dump(self.dataForMessage, json_file, indent=2, sort_keys=True)
			print("Data added to database")

			self.publish_info_to_things_board(self.parameters_to_send_with_dash7) #maybe in another theard

			self.fingerprinting(self.messageOfValues)

		self.messageOfValues = {}
		self.messageOfValues = {tempGateway : rxLevel}
            	self.old_index = parameters[12]
		#self.old_parameters = [] #
            	#self.old_parameters = parameters #
            	self.counter_of_messages = self.counter_of_messages + 1

		temp_byte = [parameters[0], parameters[1], parameters[2], parameters[3]]
		humi_byte = [parameters[4], parameters[5], parameters[6], parameters[7]]
		CO2_byte = [parameters[8], parameters[9], parameters[10], parameters[11]]

		temperature = struct.unpack('<f', bytearray(temp_byte))
		humidity = struct.unpack('<f', bytearray(humi_byte))
		CO2 = struct.unpack('<f', bytearray(CO2_byte))
		self.parameters_to_send_with_dash7 = []
		self.parameters_to_send_with_dash7 = [temperature, humidity, CO2]

            	message_number = 'message' + str(self.counter_of_messages)
            	my_details = {'temperature': temperature, 'humidity' : humidity, 'CO2 level' : CO2 }
		self.dataForMessage.clear()
            	self.dataForMessage[message_number] = [my_details]
            	self.dataForMessage[message_number].append({ 'gateway' : gateway, 'rxLevel' : rxLevel })
		print("data = ", self.messageOfValues)
	else:
            	message_number = 'message' + str(self.counter_of_messages)
            	self.dataForMessage[message_number].append({ 'gateway' : gateway, 'rxLevel' : rxLevel })
		self.messageOfValues[tempGateway] = rxLevel
		print("data = ", self.messageOfValues)


ms = mainservice();

