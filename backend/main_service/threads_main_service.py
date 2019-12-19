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
import send_email
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
        self.main_message = {}
        self.counter_of_messages = 0
        self.counter_of_messages_Jola = 0
        self.counter_of_messages_Jan = 0
        self.counter_of_messages_Ruben = 0
        self.old_index = -1
        self.old_index_Jola = -1
        self.old_index_Jan = -1
        self.old_index_Ruben = -1
        self.dataForMessage = {}
        self.dataForMessage_Jola = {}
        self.dataForMessage_Jan = {}
        self.dataForMessage_Ruben = {}
        self.messageOfValues = {}
        self.messageOfValues_Jola = {}
        self.messageOfValues_Jan = {}
        self.messageOfValues_Ruben = {}
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
        self.parameters_to_send_with_dash7_Jola = [0, 0, 0]
        self.parameters_to_send_with_dash7_Jan = [0, 0, 0]
        self.parameters_to_send_with_dash7_Ruben = [0, 0, 0]
        self.time_of_changes = {'A' : -1, 'B' : -1, 'C' : -1,  'D' : -1}
        self.times = []
        self.times_Jola = []
        self.times_Jan = []
        self.times_Ruben = []
        self.running = 0
        self.tempDevId = 'aa'
        self.currentPoint = []
        self.stop_threads = False
        self.es = send_email.email_sender() 
        self.emergency_messages = [0, 0, 0] # messages of [Jola, Jan, Ruben]
        self.all_senders = False

        print("Starting program")
        self.client.on_message = self.on_message # attach function to callback
        print("Connecting to the broker DASH7")
        try:
            self.client.connect(self.broker_address) # connect broker
        except:
            print("")
            print("Can not connect to the server!")
            print("")
            exit(1)
        print("Server connection succeeded")
        self.client.loop_start() # start the loop # loop_forever()
        print("Subscribing to topic", "/d7/#")
        self.subscribe_to_our_devices()

        # lora 
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
                time.sleep(1)

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
            if(len(self.times_Jola) != 0):
                if(( (time1 - self.times_Jola[len(self.times)-1]) > 10 or len(self.messageOfValues_Jola) == 4) and len(self.times_Jola) != 0): 
                    print("=================!!!!!!!!!!! second thread Jola", time1)
                    self.checking_missing_values('octa-jola')
                    self.saving_message('octa-jola')                
            if(len(self.times_Jan) != 0):
                if(( (time1 - self.times_Jan[len(self.times)-1]) > 10 or len(self.messageOfValues_Jan) == 4) and len(self.times_Jan) != 0):
                    print("=================!!!!!!!!!!! second thread Jan", time1)
                    self.checking_missing_values('octa-jan')
                    self.saving_message('octa-jan')                
            if(len(self.times_Ruben) != 0):
                if( ((time1 - self.times_Ruben[len(self.times)-1]) > 10 or len(self.messageOfValues_Ruben) == 4) and len(self.times_Ruben) != 0):
                    print("=================!!!!!!!!!!! second thread Ruben", time1)
                    self.checking_missing_values('octa-ruben')
                    self.saving_message('octa-ruben')                

            time.sleep(0.1)
            if self.stop_threads:
              break

    def checking_missing_values(self, tempDevId):
        #print("in checking")
        self.main_message = {}
        self.dataForMessage = {}
        self.parameters_to_send_with_dash7 = {}
        if(tempDevId == 'octa-jola'):
            self.main_message = self.messageOfValues_Jola
            self.messageOfValues_Jola = {}
            message_number = "Jola's message" + str(self.counter_of_messages_Jola)
            self.dataForMessage = self.dataForMessage_Jola
            self.parameters_to_send_with_dash7 = self.parameters_to_send_with_dash7_Jola
            self.times_Jola = []
            #print(message_number)
        elif(tempDevId == 'octa-jan'):
            self.main_message = self.messageOfValues_Jan
            self.messageOfValues_Jan = {}
            message_number = "Jan's message" + str(self.counter_of_messages_Jan)
            self.dataForMessage = self.dataForMessage_Jan
            self.parameters_to_send_with_dash7 = self.parameters_to_send_with_dash7_Jan
            self.times_Jan = []
            #print(message_number)
        elif(tempDevId == 'octa-ruben'):
            self.main_message = self.messageOfValues_Ruben
            self.messageOfValues_Ruben = {}
            message_number = "Ruben's message" + str(self.counter_of_messages_Ruben)
            self.dataForMessage = self.dataForMessage_Ruben
            self.parameters_to_send_with_dash7 = self.parameters_to_send_with_dash7_Ruben
            #print(message_number)
            self.times_Ruben = []

        #print("checking main message in checking", self.main_message)
        #print("length", len(self.main_message))
        if(len(self.main_message) < 4 and len(self.main_message) > 0):
            print("Missing messages from gateways")
            if ('A' not in self.main_message):
                #print("missing a")
                self.main_message['A'] = 120
                #print("before append", self.main_message)
                #print("message number", message_number)
                self.dataForMessage[message_number].append({ 'gateway' : self.A, 'rxLevel' : 120 })
                #print("missing a _ main_message", self.main_message)
            if ('B' not in self.main_message):
                #print("missing b")
                self.main_message['B'] = 120
                #print("before append", self.main_message)
                #print("message number", message_number)
                self.dataForMessage[message_number].append({ 'gateway' : self.B, 'rxLevel' : 120 })
                #print("missing b _ main_message", self.main_message)
            if ('C' not in self.main_message):
                #print("missing c")
                self.main_message['C'] = 120
                #print("before append", self.main_message)
                #print("message number", message_number)
                self.dataForMessage[message_number].append({ 'gateway' : self.C, 'rxLevel' : 120 })
                #print("missing c _ main_message", self.main_message)
            if ('D' not in self.main_message):
                #print("missing d")
                self.main_message['D'] = 120
                #print("before append", self.main_message)
                #print("message number", message_number)
                self.dataForMessage[message_number].append({ 'gateway' : self.D, 'rxLevel' : 120 })
                #print("missing d _ main_message", self.main_message)
            #print ("in if datas = ", self.main_message)
            print(message_number, self.main_message)
        
    def saving_message(self, tempDevId):
        #print("in saving message")
        if(len(self.main_message) == 4 and self.main_message != 0):
            with open('measurements.json', 'a') as json_file:
                json.dump(self.dataForMessage, json_file, indent=2, sort_keys=True)
            print("Data from DASH7 added to database")
            self.fingerprinting(self.main_message)
            self.publish_info_to_things_board(self.parameters_to_send_with_dash7, self.tempDevId, 'DASH7')
            time.sleep(2)
    
    def uplink_callback(self,msg, client):
        print("Uplink started")
        #print("Received uplink from ", msg.dev_id)
        temp_lora = msg.payload_fields.temp
        humi_lora = msg.payload_fields.humid
        CO2_lora = msg.payload_fields.CO2
        TVOC_lora = msg.payload_fields.TVOC
        danger_lora = msg.payload_fields.Danger
        emergency_lora = msg.payload_fields.Emergency
        #print("counter: ", msg.counter)  
        self.counter_of_messages = self.counter_of_messages + 1
        message_number = self.counter_of_messages
        data_from_lora = [temp_lora, humi_lora, CO2_lora, TVOC_lora, danger_lora, emergency_lora] 
        my_details_lora = {'temperature': temp_lora, 'humidity' : humi_lora, 'CO2 level' : CO2_lora, 'TVOC level' : TVOC_lora, 'danger' : danger_lora, 'emergency' : emergency_lora}
        dataFromLora[message_number] = [my_details_lora]
        print("data to save in database from lora", dataFromLora)
        with open('measurements.json', 'a') as json_file:
            json.dump(dataFromLora, json_file, indent=2, sort_keys=True)
        print("Data from Lora added to database")
        self.publish_info_to_things_board(data_from_lora, msg.dev_id, 'Lora')

    def publish_info_to_things_board(self, data, dev_id, typeOfMessage):
        #print("in publishing")
        number_of_message = 0
        mail_message = ""
        if(dev_id == u'octa-ruben'):
            #print('ruben')
            access_token = 'P6S9p9cWsikzbqRNgQ7f' #Ruben
            mail_message = "Ruben in danger"
            self.emergency_messages[2] = self.emergency_messages[2] + 1
            number_of_message = self.emergency_messages[2]
            if(self.emergency_messages[2] >= 4):
                self.emergency_messages[2] = 0
        if(dev_id == u'octa-jola'):
            #print('jola')
            access_token = 'wFRAZQnB2t8hIol30gkm' #Jola
            mail_message = "Jola in danger"
            self.emergency_messages[0] = self.emergency_messages[0] + 1
            number_of_message = self.emergency_messages[0]
            if(self.emergency_messages[0] >= 4):
                self.emergency_messages[0] = 0
        elif(dev_id == u'octa-jan'):
            #print('jan')
            access_token = 'IWjQcWm3Q2PEiY9duRQP' #Jan
            mail_message = "Jan in danger"
            self.emergency_messages[1] = self.emergency_messages[1] + 1
            number_of_message = self.emergency_messages[1]
            if(self.emergency_messages[1] >= 4):
                self.emergency_messages[1] = 0
        broker = "thingsboard.idlab.uantwerpen.be"
        topic = "v1/devices/me/telemetry"
        self.client_pub.username_pw_set(access_token)
        try:
            self.client_pub.connect(broker, keepalive=20)
        except:
            print("Connection to the things board failed!")
            #print(access_token)
            try:
                self.client_pub.connect(broker, keepalive=20)
            except:
                print("Connection to the things board failed!")
                return(1)
        #print("--------- -> connected to things board")
        
        if(typeOfMessage == 'DASH7'): 
            if(self.parameters_to_send_with_dash7[4] == 1 and self.parameters_to_send_with_dash7[5] == 0): # danger mode 
                message_to_send = {"temperature" : data[0], "humidity" : data[1], "CO2" : data[2], "TVOC" : data[3], "is_danger" : data[4], "is_emergency" : data[5], "x" : self.currentPoint[0], "y" : self.currentPoint[1]}
                #print("-------------Dev ID DASH7 = ", dev_id)
            elif(self.parameters_to_send_with_dash7[4] == 1 and self.parameters_to_send_with_dash7[5] == 1): # emergency mode
                message_to_send = {"x" : self.currentPoint[0], "y" : self.currentPoint[1]}
                if(number_of_message == 1):
                    print("*************** EMERGENCY MODE ********************")
                    self.es.send_email(self.all_senders, "EMERGENCY ALERT", mail_message)
                    message_to_send = {"temperature" : data[0], "humidity" : data[1], "CO2" : data[2], "TVOC" : data[3], "is_danger" : data[4], "is_emergency" : data[5], "x" : self.currentPoint[0], "y" : self.currentPoint[1]}
        elif(typeOfMessage == 'Lora'):
            #print("-------------Dev ID Lora = ", dev_id)
            message_to_send = {"temperature" : data[0], "humidity" : data[1], "CO2" : data[2], "TVOC" : data[3], "is_danger" : data[4], "is_emergency" : data[5]}
        result = json.dumps(message_to_send)
        self.client_pub.publish(topic, result)
        print("Data = ", message_to_send)
        print("Data published successfully to ThingsBoard!")
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
        #self.kn.plotmap(point) # displaying matlab map
        print("Fingerprinting done correctly")

    def on_message(self,client, userdata, message):
        print(" ")
        print(" ")
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
        device_id_dash7 = parser.getDeviceId()
        self.tempDevId = ""
        tempGateway = self.gw[gateway]
        time_now = ((datetime.datetime.now().time().minute)*60) + datetime.datetime.now().time().second
        if(device_id_dash7 == '5203408228350296120'):
            self.tempDevId = 'octa-jola'
            self.times_Jola.append(time_now)
            old_time = self.times_Jola[len(self.times)-1]
            #print("times Jola= ", self.times_Jola)
            #print("old_index Jola", self.old_index_Jola)
        elif(device_id_dash7 == '5203408228352000069'):
            self.tempDevId = 'octa-ruben'
            self.times_Ruben.append(time_now)
            old_time = self.times_Ruben[len(self.times)-1]
            #print("times Ruben = ", self.times_Ruben)
            #print("old_index Ruben", self.old_index_Ruben)
        elif(device_id_dash7 == '5203408228351803455'):
            self.tempDevId = 'octa-jan'
            self.times_Jan.append(time_now)
            old_time = self.times_Jan[len(self.times)-1]
            #print("times Jan= ", self.times_Jan)
            #print("old_index Jan", self.old_index_Jan)

        #print("param[13]", parameters[13])
        
        if( ((self.tempDevId == 'octa-jola') and ((self.old_index_Jola < 0) or (parameters[13] != self.old_index_Jola))) or ((self.tempDevId == 'octa-ruben') and ((self.old_index_Ruben < 0) or (parameters[13] != self.old_index_Ruben))) or ((self.tempDevId == 'octa-jan') and ((self.old_index_Jan < 0) or (parameters[13] != self.old_index_Jan))) ):
            
            if((self.tempDevId == 'octa-jola' and len(self.messageOfValues_Jola) > 0) or (self.tempDevId == 'octa-ruben' and len(self.messageOfValues_Ruben) > 0) or (self.tempDevId == 'octa-jan' and len(self.messageOfValues_Jan) > 0)):
                #print("--------------saving message before second thread")
                self.checking_missing_values(self.tempDevId)
                self.saving_message(self.tempDevId)
                if(self.tempDevId == 'octa-jola'):
                    self.times_Jola.append(old_time)
                elif(self.tempDevId == 'octa-jan'):
                    self.times_Jan.append(old_time)
                elif(self.tempDevId == 'octa-ruben'):
                    self.times_Ruben.append(old_time)
            
            #print("before param")
            temp_byte = [parameters[0], parameters[1], parameters[2], parameters[3]]
            humi_byte = [parameters[4], parameters[5], parameters[6], parameters[7]]
            CO2 = parameters[8] + (parameters[9])*256
            TVOC = parameters[10] + (parameters[11])*256

            #print("--------!!!!-------- param[12] = ", parameters[12])
            if(parameters[12] == 0):
                #print("0")
                emergency = 0
                danger = 0
            elif(parameters[12] == 1):
                #print("1")
                emergency = 0
                danger = 1
            elif(parameters[12] == 3):
                #print("3")
                emergency = 1
                danger = 1
            else:
                print("Wrong values of 'danger' and 'emergency'")

            temperature = struct.unpack('<f', bytearray(temp_byte))[0]
            humidity = struct.unpack('<f', bytearray(humi_byte))[0]

            if(self.tempDevId == 'octa-jola'):
                self.messageOfValues_Jola = {tempGateway : rxLevel}
                self.old_index_Jola = parameters[13]
                self.counter_of_messages = self.counter_of_messages + 1
                self.counter_of_messages_Jola = self.counter_of_messages
                self.parameters_to_send_with_dash7_Jola = []
                self.parameters_to_send_with_dash7_Jola = [temperature, humidity, CO2, TVOC, danger, emergency]
                message_number = "Jola's message" + str(self.counter_of_messages_Jola)
                my_details = {'temperature': temperature, 'humidity' : humidity, 'CO2 level' : CO2, 'TVOC level' : TVOC, 'danger' : danger, 'emergency' : emergency}
                self.dataForMessage_Jola.clear()
                self.dataForMessage_Jola[message_number] = [my_details]
                self.dataForMessage_Jola[message_number].append({ 'gateway' : gateway, 'rxLevel' : rxLevel })
                #print("msg number Jola", message_number)
                print(message_number, my_details)
                print(message_number, self.messageOfValues_Jola)
            elif(self.tempDevId == 'octa-jan'):
                self.messageOfValues_Jan = {tempGateway : rxLevel}
                self.old_index_Jan = parameters[13]
                self.counter_of_messages = self.counter_of_messages + 1
                self.counter_of_messages_Jan = self.counter_of_messages
                self.parameters_to_send_with_dash7_Jan = []
                self.parameters_to_send_with_dash7_Jan = [temperature, humidity, CO2, TVOC, danger, emergency]
                message_number = "Jan's message" + str(self.counter_of_messages_Jan)
                my_details = {'temperature': temperature, 'humidity' : humidity, 'CO2 level' : CO2, 'TVOC level' : TVOC, 'danger' : danger, 'emergency' : emergency}
                self.dataForMessage_Jan.clear()
                self.dataForMessage_Jan[message_number] = [my_details]
                self.dataForMessage_Jan[message_number].append({ 'gateway' : gateway, 'rxLevel' : rxLevel })
                #print("msg number Jan", message_number)
                print(message_number, self.my_details)
                print(message_number, self.messageOfValues_Jan)
            elif(self.tempDevId == 'octa-ruben'):
                self.messageOfValues_Ruben = {tempGateway : rxLevel}
                self.old_index_Ruben = parameters[13]
                self.counter_of_messages = self.counter_of_messages + 1
                self.counter_of_messages_Ruben = self.counter_of_messages
                self.parameters_to_send_with_dash7_Ruben = []
                self.parameters_to_send_with_dash7_Ruben = [temperature, humidity, CO2, TVOC, danger, emergency]
                message_number = "Ruben's message" + str(self.counter_of_messages_Ruben)
                my_details = {'temperature': temperature, 'humidity' : humidity, 'CO2 level' : CO2, 'TVOC level' : TVOC, 'danger' : danger, 'emergency' : emergency}
                self.dataForMessage_Ruben.clear()
                self.dataForMessage_Ruben[message_number] = [my_details]
                self.dataForMessage_Ruben[message_number].append({ 'gateway' : gateway, 'rxLevel' : rxLevel })
                #print("msg number Ruben", message_number)
                print(message_number, my_details)
                print(message_number, self.messageOfValues_Ruben)
        else:
            #print("----- in else")
            if(self.tempDevId == 'octa-jola'):
                message_number = "Jola's message" + str(self.counter_of_messages_Jola)
                self.dataForMessage_Jola[message_number].append({ 'gateway' : gateway, 'rxLevel' : rxLevel })
                self.messageOfValues_Jola[tempGateway] = rxLevel
                #print("msg number Jola", message_number)
                print(message_number, self.messageOfValues_Jola)
            elif(self.tempDevId == 'octa-jan'):
                message_number = "Jan's message" + str(self.counter_of_messages_Jan)
                self.dataForMessage_Jan[message_number].append({ 'gateway' : gateway, 'rxLevel' : rxLevel })
                self.messageOfValues_Jan[tempGateway] = rxLevel
                #print("msg number Jan", message_number)
                print(message_number, self.messageOfValues_Jan)
            elif(self.tempDevId == 'octa-ruben'):
                message_number = "Ruben's message" + str(self.counter_of_messages_Ruben)
                self.dataForMessage_Ruben[message_number].append({ 'gateway' : gateway, 'rxLevel' : rxLevel })
                self.messageOfValues_Ruben[tempGateway] = rxLevel
                #print("msg number Ruben", message_number)
                print(message_number, self.messageOfValues_Ruben)

ms = mainservice();
