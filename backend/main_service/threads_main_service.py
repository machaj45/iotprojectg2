import paho.mqtt.client as mqtt # import the client1
import time
import ttn
import argparse
import json
import struct
import sys
import datetime
import threading
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
	self.time_of_changes = {'A' : -1, 'B' : -1, 'C' : -1,  'D' : -1}
	self.times = []
        self.running = 0
        self.tempDevId = 'aa'
        self.currentPoint = []
        self.stop_threads = False

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

        try:
            self.t = threading.Thread(name='second thread', target=self.checking)
            self.t.start()

            while True:
                self.running = 1

        except KeyboardInterrupt:
            self.running = 0
            self.client.loop_stop() # stop the loop for dash7 listening
            self.mqtt_client_lora.close()
            self.stop_threads = True
            self.t.join()
            print("Script interrupted!")

    def subscribe_to_our_devices(self):
        # This is device of Jan
        self.client.subscribe("/d7/483638370041003f/#")
        # This is device of Ruben
        self.client.subscribe("/d7/4836383700440045/#")
        # This is device of Jola
        self.client.subscribe("/d7/48363837002a0038/#")

    def checking(self):
        while True:
            time1 = ((datetime.datetime.now().time().minute)*60) + datetime.datetime.now().time().second
            if(len(self.times) != 0):
                if( (time1 - self.times[len(self.times)-1]) > 6):
                    if(len(self.messageOfValues) < 4 and len(self.messageOfValues) > 0):
                        #print("Missing messages from gateways")
                        message_number = 'message' + str(self.counter_of_messages)
                        if ('A' not in self.messageOfValues):
                            self.messageOfValues['A'] = 120
                            self.dataForMessage[message_number].append({ 'gateway' : self.A, 'rxLevel' : 120 })
                        if ('B' not in self.messageOfValues):
                            self.messageOfValues['B'] = 120
                            self.dataForMessage[message_number].append({ 'gateway' : self.B, 'rxLevel' : 120 })
                        if ('C' not in self.messageOfValues):
                            self.messageOfValues['C'] = 120
                            self.dataForMessage[message_number].append({ 'gateway' : self.C, 'rxLevel' : 120 })
                        if ('D' not in self.messageOfValues):
                            self.messageOfValues['D'] = 120
                            self.dataForMessage[message_number].append({ 'gateway' : self.D, 'rxLevel' : 120 })
                
                if(len(self.messageOfValues) == 4 and self.messageOfValues != 0):
                    with open('measurements.json', 'a') as json_file:
                        json.dump(self.dataForMessage, json_file, indent=2, sort_keys=True)
                    #print("Data added to database")
                    
                    self.fingerprinting(self.messageOfValues)
                    
                    self.publish_info_to_things_board(self.parameters_to_send_with_dash7, self.tempDevId, 'DASH7')
                    
                    self.times = []
                    self.messageOfValues = {}
                    time.sleep(2)

            if self.stop_threads:
                break

    def uplink_callback(self,msg, client):
        print("Uplink started")
        print("Received uplink from ", msg.dev_id)
	temp_lora = msg.payload_fields.temp
	humi_lora = msg.payload_fields.humid
	CO2_lora = msg.payload_fields.CO2
        TVOC_lora = msg.payload_fields.TVOC
        danger_lora = msg.payload_fields.Danger
        emergency_lora = msg.payload_fields.Emergency
        #print("counter: ", msg.counter)
	data_from_lora = [temp_lora, humi_lora, CO2_lora, TVOC_lora, danger_lora, emergency_lora]
	self.publish_info_to_things_board(data_from_lora, msg.dev_id, 'Lora')

#    def on_connect(client, userdata, flags, rc):
#	print("Connection returned result: " + connack_string(rc))

#    def on_publish(client, userdata, result):
#	print("data published")

#    def on_disconnect(client, userdata, rc):
#	print("client disconnected ok")

    def publish_info_to_things_board(self, data, dev_id, typeOfMessage):
        print("in publishing")
        if(dev_id == u'octa-ruben'):
            access_token = 'P6S9p9cWsikzbqRNgQ7f' #Ruben
            print('ruben')
        if(dev_id == u'octa-jola'):
            print('jola')
            access_token = 'wFRAZQnB2t8hIol30gkm' #Jola
        elif(dev_id == u'octa-jan'):
            print('jan')
            access_token = 'IWjQcWm3Q2PEiY9duRQP' #Jan
	broker = "thingsboard.idlab.uantwerpen.be"
	topic = "v1/devices/me/telemetry"
	#self.client_pub.on_connect = on_connect
	#self.client_pub.on_publish = on_publish
	self.client_pub.username_pw_set(access_token)
	self.client_pub.connect(broker, keepalive=20)
        print("!!!!! is danger",data[4]) #check if it is ok
        print("!!!!! is emergency", data[5]) #check if it is ok
        if(typeOfMessage == 'DASH7'):
            message_to_send = {"temperature" : data[0], "humidity" : data[1], "CO2" : data[2], "TVOC" : data[3], "is_danger" : data[4], "is_emergency" : data[5], "x" : self.currentPoint[0], "y" : self.currentPoint[1]}
            print("-------------Dev ID DASH7 = ", dev_id)
        elif(typeOfMessage == 'Lora'):
            print("-------------Dev ID Lora = ", dev_id)
            message_to_send = {"temperature" : data[0], "humidity" : data[1], "CO2" : data[2], "TVOC" : data[3], "is_danger" : data[4], "is_emergency" : data[5]}
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
        self.currentPoint = point[0]
        print(self.currentPoint)
	#self.kn.plotmap(point)
	print("Fingerprinting done correctly")

    def on_message(self,client, userdata, message):
        print(" ")
	print("on message")
	topic = message.topic
        gateway =  topic.split("/")[3]
        message1 = str(message.payload.decode("utf-8"))
	hexstring = message1.strip().replace(' ', '')
	data = bytearray(hexstring.decode("hex"))
        parser = AlpParser()
        parsed_values = parser.parse(ConstBitStream(data), len(data))
        #print(parsed_values)
        parameters = parser.getData()
        rxLevel = parser.getRxLevel()
        device_id_dash7 = parser.getDeviceId()
        self.tempDevId = ""
        if(device_id_dash7 == '5203408228350296120'):
            self.tempDevId = 'octa-jola'
        elif(device_id_dash7 == '5203408228352000069'):
            self.tempDevId = 'octa-ruben'
        elif(device_id_dash7 == '5203408228351803455'):
            self.tempDevId = 'octa-jan'
	else:
	    print("Missing dev id!!!! = ", device_id_dash7)
        tempGateway = self.gw[gateway]
        time_now = ((datetime.datetime.now().time().minute)*60) + datetime.datetime.now().time().second
	self.times.append(time_now)
	print("times = ", self.times)
	if((self.old_index < 0) or (parameters[13] != self.old_index)):
		self.messageOfValues = {tempGateway : rxLevel}
            	self.old_index = parameters[13]
            	self.counter_of_messages = self.counter_of_messages + 1

		temp_byte = [parameters[0], parameters[1], parameters[2], parameters[3]]
		humi_byte = [parameters[4], parameters[5], parameters[6], parameters[7]]
		CO2 = parameters[8] + (parameters[9])*256
		TVOC = parameters[10] + (parameters[11])*256
		if(parameters[12] == 0):
			emergency = 0
			danger = 0
		elif(parameters[12] == 1):
			emergency = 0
			danger = 1
		elif(parameters[12] == 3):
			emergency = 1
			danger = 1                

#danger = parameters[12]
 #               emergency = (parameters[12]&0b00000010)/2
                #print("!!!!! is danger",danger) #check if it is ok
                #print("!!!!! is emergency", emergency) #check if it is ok
		
                temperature = struct.unpack('<f', bytearray(temp_byte))[0]
		humidity = struct.unpack('<f', bytearray(humi_byte))[0]
		self.parameters_to_send_with_dash7 = []
		self.parameters_to_send_with_dash7 = [temperature, humidity, CO2, TVOC, danger, emergency]

            	message_number = 'message' + str(self.counter_of_messages)
                my_details = {'temperature': temperature, 'humidity' : humidity, 'CO2 level' : CO2, 'TVOC level' : TVOC}
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

