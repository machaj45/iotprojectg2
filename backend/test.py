import time
import ttn

app_id = "theultimateapplication"
access_key = "ttn-account-v2.gGuc6mi3k6sF6W19mIYw1cOvkMjfMLlseSOT6LPtXRw"

def uplink_callback(msg, client):
  print("Received uplink from ", msg.dev_id)
  print(msg.payload_raw)

handler = ttn.HandlerClient(app_id, access_key)

# using mqtt client
mqtt_client = handler.data()
mqtt_client.set_uplink_callback(uplink_callback)
mqtt_client.connect()
mqtt_client.send(dev_id="exploreremachygreen",  pay= "AQ==", port=1, conf=True, sched="replace") # send 0x01
time.sleep(10)
mqtt_client.close()
time.sleep(1)
mqtt_client.connect()
mqtt_client.send(dev_id="exploreremachygreen",  pay= "AQ==", port=1, conf=True, sched="replace") # send 0x00
time.sleep(10)
mqtt_client.close()
# using application manager client
app_client =  handler.application()
#my_app = app_client.get()
#print(my_app)
#my_devices = app_client.devices()
#print(my_devices)
