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

class mainservice:
    counter_of_messages = 0;
    old_parameters = []
    dataForMessage = {}
    broker_address = "student-04.idlab.uantwerpen.be"
    client = mqtt.Client("P1") # create new instance 
    def __init__(self):
        print("python main function")
        print("creating new instance")
        self.client.on_message=self.on_message # attach function to callback
        print("connecting to the broker")
        self.client.connect(self.broker_address) # connect broker
        self.client.loop_start() # start the loop
        print("Subscribing to topic", "/d7/#")
        self.subscribe_to_our_devices()
        time.sleep(1000) # wait
        self.client.loop_stop() # stop the loop

    def subscribe_to_our_devices(self):
        # This is device of Jan
        self.client.subscribe("/d7/483638370041003f/#")
        # This is device of Ruben
        self.client.subscribe("/d7/4836383700440045/#")
        # This is device of Jola
        self.client.subscribe("/d7/48363837002a0038/#")

    def on_message(self,client, userdata, message):
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


        if((self.old_parameters == []) or (parameters != self.old_parameters)):
            with open('database.json', 'a') as json_file:
                    json.dump(self.dataForMessage, json_file)
            print("data added to database")
            print("--------first case")
            self.old_parameters = []
            self.old_parameters = parameters
            self.counter_of_messages = self.counter_of_messages + 1
            message_number = 'message' + str(self.counter_of_messages)
            my_details = {'data1': parameters[0], 'data2' : parameters[1], 'data3' : parameters[2] }
            print(my_details)
            print("----------------------------")
            self.dataForMessage.clear()
            self.dataForMessage[message_number] = [my_details]
            self.dataForMessage[message_number].append({ 'gateway' : gateway, 'rxLevel' : rxLevel })
            print("last thing in first cache----------")
            print(self.dataForMessages)
            print("-----------------------")
        else:
            print("!!!!!     another case")
            print("--------------------")
            message_number = 'message' + str(self.counter_of_messages)
            print(message_number)
            #data = {}
            print("---------------------")
            print(self.dataForMessage)
            self.dataForMessage[message_number].append({ 'gateway' : gateway, 'rxLevel' : rxLevel })
            print("----------")
            print(self.dataForMessage)
            print("---before data --")
            print(data)
            print("----after data----")
ms = mainservice();

